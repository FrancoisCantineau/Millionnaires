/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldIndicatorComponent" - Source
 * Notes: Binds to DispatchMissionManagerComponent and drives world indicators (site highlight + world notifications).
 */
#include "Dispatch/Map/DispatchMissionWorldIndicatorComponent.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Map/DispatchMissionSiteActor.h"
#include "Dispatch/Map/DispatchMissionNotificationPoint.h"
#include "Dispatch/Map/DispatchMissionOfferIndicatorActor.h"
#include "Dispatch/Map/DispatchMissionOfferClickProxyComponent.h"

#include "Engine/World.h"
#include "EngineUtils.h"

UDispatchMissionWorldIndicatorComponent::UDispatchMissionWorldIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDispatchMissionWorldIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bDebugLogs)
	{
		UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] BeginPlay | Owner=%s"), *GetNameSafe(GetOwner()));
	}

	BindToMissionManager();
}

#pragma region INTERNAL

void UDispatchMissionWorldIndicatorComponent::BindToMissionManager()
{
	if (missionManager.IsValid())
	{
		return;
	}

	AActor* Owner = GetOwner();
	missionManager = Owner ? Owner->FindComponentByClass<UDispatchMissionManagerComponent>() : nullptr;

	if (!missionManager.IsValid() && bFallbackSearchWorldForMissionManager)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (UDispatchMissionManagerComponent* Found = It->FindComponentByClass<
					UDispatchMissionManagerComponent>())
				{
					missionManager = Found;
					break;
				}
			}
		}
	}

	if (!missionManager.IsValid())
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "[DispatchWorldIndicators] MissionManager not found. Add this component on the SAME Owner as UDispatchMissionManagerComponent (recommended: Dispatch PlayerController)."));
		return;
	}

	missionManager->OnOfferAdded.AddDynamic(this, &UDispatchMissionWorldIndicatorComponent::HandleOfferAdded);
	missionManager->OnOfferRemoved.AddDynamic(this, &UDispatchMissionWorldIndicatorComponent::HandleOfferRemoved);
	missionManager->OnMissionStateChanged.AddDynamic(
		this, &UDispatchMissionWorldIndicatorComponent::HandleMissionStateChanged);

	if (bDebugLogs)
	{
		UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] Bound | Owner=%s | MissionManagerOwner=%s"),
		       *GetNameSafe(GetOwner()), *GetNameSafe(missionManager->GetOwner()));
	}
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::PickBestSite(
	EDispatchMissionLocation DesiredLocation, const FVector& NearWorldLocation, int32 Seed) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<ADispatchMissionSiteActor*> AllSites;
	for (TActorIterator<ADispatchMissionSiteActor> It(World); It; ++It)
	{
		AllSites.Add(*It);
	}

	if (AllSites.Num() == 0)
	{
		return nullptr;
	}

	// Exact matches
	if (DesiredLocation != EDispatchMissionLocation::Any)
	{
		TArray<ADispatchMissionSiteActor*> Exact;
		for (ADispatchMissionSiteActor* S : AllSites)
		{
			if (S && S->GetMissionLocation() == DesiredLocation)
			{
				Exact.Add(S);
			}
		}

		if (Exact.Num() > 0)
		{
			FRandomStream Rng(Seed);
			return Exact[Rng.RandRange(0, Exact.Num() - 1)];
		}
	}

	// Any sites
	TArray<ADispatchMissionSiteActor*> AnySites;
	for (ADispatchMissionSiteActor* S : AllSites)
	{
		if (S && S->GetMissionLocation() == EDispatchMissionLocation::Any)
		{
			AnySites.Add(S);
		}
	}

	if (AnySites.Num() > 0)
	{
		FRandomStream Rng(Seed);
		return AnySites[Rng.RandRange(0, AnySites.Num() - 1)];
	}

	// Nearest fallback
	ADispatchMissionSiteActor* Best = AllSites[0];
	double BestDist2 = FVector::DistSquared(AllSites[0]->GetActorLocation(), NearWorldLocation);

	for (ADispatchMissionSiteActor* S : AllSites)
	{
		if (!S) continue;
		const double D2 = FVector::DistSquared(S->GetActorLocation(), NearWorldLocation);
		if (D2 < BestDist2)
		{
			BestDist2 = D2;
			Best = S;
		}
	}

	return Best;
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::ResolveSiteFromOffer(
	const FGuid& OfferId, FDispatchMissionOffer* OutOffer)
{
	if (!missionManager.IsValid())
	{
		return nullptr;
	}

	FDispatchMissionOffer Offer;
	if (!missionManager->TryGetOffer(OfferId, Offer))
	{
		return nullptr;
	}

	if (OutOffer)
	{
		*OutOffer = Offer;
	}

	if (Offer.locationActor)
	{
		return Offer.locationActor;
	}

	return PickBestSite(Offer.missionLocation, Offer.worldLocation, Offer.seed);
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::ResolveSiteFromMission(
	const FGuid& MissionId, FDispatchActiveMission* OutMission)
{
	if (!missionManager.IsValid())
	{
		return nullptr;
	}

	FDispatchActiveMission Mission;
	if (!missionManager->TryGetActiveMission(MissionId, Mission))
	{
		return nullptr;
	}

	if (OutMission)
	{
		*OutMission = Mission;
	}

	if (Mission.locationActor)
	{
		return Mission.locationActor;
	}

	// If we still have offer mapping, use it
	if (const TWeakObjectPtr<ADispatchMissionSiteActor>* FromOffer = offerToSite.Find(Mission.sourceOfferId))
	{
		if (FromOffer && FromOffer->IsValid())
		{
			return FromOffer->Get();
		}
	}

	return PickBestSite(Mission.missionLocation, Mission.worldLocation, Mission.seed);
}

ADispatchMissionNotificationPoint* UDispatchMissionWorldIndicatorComponent::PickNotificationPoint(
	EDispatchMissionLocation Location, const FVector& NearWorldLocation, int32 Seed) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<ADispatchMissionNotificationPoint*> Points;
	for (TActorIterator<ADispatchMissionNotificationPoint> It(World); It; ++It)
	{
		Points.Add(*It);
	}

	if (Points.Num() == 0)
	{
		return nullptr;
	}

	// Prefer exact matches, then Any points.
	TArray<ADispatchMissionNotificationPoint*> Exact;
	TArray<ADispatchMissionNotificationPoint*> Any;

	for (ADispatchMissionNotificationPoint* P : Points)
	{
		if (!P) continue;
		if (P->GetMissionLocation() == Location)
		{
			Exact.Add(P);
		}
		else if (P->GetMissionLocation() == EDispatchMissionLocation::Any)
		{
			Any.Add(P);
		}
	}

	auto PickNearest = [&](const TArray<ADispatchMissionNotificationPoint*>& List) -> ADispatchMissionNotificationPoint*
	{
		if (List.Num() == 0) return nullptr;

		ADispatchMissionNotificationPoint* Best = List[0];
		double BestDist2 = FVector::DistSquared(Best->GetActorLocation(), NearWorldLocation);

		for (ADispatchMissionNotificationPoint* P : List)
		{
			if (!P) continue;
			const double D2 = FVector::DistSquared(P->GetActorLocation(), NearWorldLocation);
			if (D2 < BestDist2)
			{
				BestDist2 = D2;
				Best = P;
			}
		}
		return Best;
	};

	if (Exact.Num() > 0)
	{
		return PickNearest(Exact);
	}

	if (Any.Num() > 0)
	{
		FRandomStream Rng(Seed);
		return Any[Rng.RandRange(0, Any.Num() - 1)];
	}

	return PickNearest(Points);
}

void UDispatchMissionWorldIndicatorComponent::SpawnOfferIndicator(const FGuid& OfferId, const FVector& WorldLocation,
                                                                  EDispatchMissionLocation Location, int32 Seed)
{
	if (!offerIndicatorActorClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Do not spawn twice.
	if (TWeakObjectPtr<ADispatchMissionOfferIndicatorActor>* Existing = offerToIndicator.Find(OfferId))
	{
		if (Existing && Existing->IsValid())
		{
			return;
		}
		offerToIndicator.Remove(OfferId);
	}

	ADispatchMissionNotificationPoint* Point = PickNotificationPoint(Location, WorldLocation, Seed);

	FVector SpawnLocation = Point ? (Point->GetActorLocation() + Point->GetWidgetOffset()) : WorldLocation;
	SpawnLocation.Z += offerIndicatorZOffset;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADispatchMissionOfferIndicatorActor* Indicator = World->SpawnActor<ADispatchMissionOfferIndicatorActor>(
		offerIndicatorActorClass, SpawnLocation, FRotator::ZeroRotator, Params);

	if (!Indicator)
	{
		return;
	}

	Indicator->Initialize(missionManager.Get(), OfferId);
	offerToIndicator.Add(OfferId, Indicator);

	if (bDebugLogs)
	{
		UE_LOG(LogTemp, Log,
		       TEXT("[DispatchWorldIndicators] SpawnOfferIndicator | OfferId=%s | Point=%s | Indicator=%s"),
		       *OfferId.ToString(), *GetNameSafe(Point), *GetNameSafe(Indicator));
	}
}

void UDispatchMissionWorldIndicatorComponent::DestroyOfferIndicator(const FGuid& OfferId)
{
	if (TWeakObjectPtr<ADispatchMissionOfferIndicatorActor>* Existing = offerToIndicator.Find(OfferId))
	{
		if (Existing && Existing->IsValid())
		{
			Existing->Get()->Destroy();
		}
	}

	offerToIndicator.Remove(OfferId);
}

void UDispatchMissionWorldIndicatorComponent::SetClickProxyForSite(ADispatchMissionSiteActor* Site,
                                                                   const FGuid* OfferIdOrNull,
                                                                   const FGuid* MissionIdOrNull)
{
	if (!bAttachClickProxiesToHighlightedActors || !Site)
	{
		return;
	}

	auto ApplyToActor = [&](AActor* TargetActor)
	{
		if (!TargetActor) return;

		UDispatchMissionOfferClickProxyComponent* Proxy = TargetActor->FindComponentByClass<
			UDispatchMissionOfferClickProxyComponent>();
		if (!Proxy)
		{
			Proxy = NewObject<UDispatchMissionOfferClickProxyComponent>(
				TargetActor, UDispatchMissionOfferClickProxyComponent::StaticClass(), NAME_None, RF_Transient);
			if (Proxy)
			{
				TargetActor->AddInstanceComponent(Proxy);
				Proxy->RegisterComponent();
			}
		}

		if (!Proxy)
		{
			return;
		}

		if (OfferIdOrNull && OfferIdOrNull->IsValid())
		{
			Proxy->SetOffer(*OfferIdOrNull, Site);
		}
		else if (MissionIdOrNull && MissionIdOrNull->IsValid())
		{
			Proxy->SetMission(*MissionIdOrNull, Site);
		}
		else
		{
			Proxy->Clear();
		}
	};

	// Apply to all configured target actors and the site itself.
	ApplyToActor(Site);

	for (AActor* A : Site->targetActors)
	{
		ApplyToActor(A);
	}
}

#pragma endregion INTERNAL

#pragma region CALLBACKS

void UDispatchMissionWorldIndicatorComponent::HandleOfferAdded(const FGuid& OfferId)
{
	if (!missionManager.IsValid())
	{
		BindToMissionManager();
	}

	FDispatchMissionOffer Offer;
	ADispatchMissionSiteActor* Site = ResolveSiteFromOffer(OfferId, &Offer);

	if (bDebugLogs)
	{
		UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] OfferAdded | OfferId=%s | Location=%d | Site=%s"),
		       *OfferId.ToString(), (int32)Offer.missionLocation, *GetNameSafe(Site));
	}

	if (!Site)
	{
		return;
	}

	Site->RebuildMeshCache();
	Site->SetOfferActive(true);

	offerToSite.Add(OfferId, Site);

	// World-space UMG indicator
	SpawnOfferIndicator(OfferId, Offer.worldLocation, Site->GetMissionLocation(), Offer.seed);

	// Click proxy on highlighted actors -> opens details menu
	SetClickProxyForSite(Site, &OfferId, nullptr);
}

void UDispatchMissionWorldIndicatorComponent::HandleOfferRemoved(const FGuid& OfferId)
{
	if (!missionManager.IsValid())
	{
		return;
	}

	DestroyOfferIndicator(OfferId);

	TWeakObjectPtr<ADispatchMissionSiteActor>* FoundSitePtr = offerToSite.Find(OfferId);
	if (!FoundSitePtr || !FoundSitePtr->IsValid())
	{
		offerToSite.Remove(OfferId);
		return;
	}

	ADispatchMissionSiteActor* Site = FoundSitePtr->Get();

	// If an active mission exists with this sourceOfferId, the offer was accepted.
	FDispatchActiveMission AcceptedMission;
	const bool bAccepted = missionManager->TryGetActiveMissionFromOffer(OfferId, AcceptedMission);

	if (bAccepted)
	{
		missionToSite.Add(AcceptedMission.missionId, Site);

		if (bDebugLogs)
		{
			UE_LOG(LogTemp, Log,
			       TEXT("[DispatchWorldIndicators] OfferRemoved (Accepted) | OfferId=%s -> MissionId=%s | Site=%s"),
			       *OfferId.ToString(), *AcceptedMission.missionId.ToString(), *GetNameSafe(Site));
		}

		// Keep highlight alive; mission state updates will drive it.
		Site->SetMissionState(AcceptedMission.state);

		// Switch click proxy to mission id (so clicking highlighted meshes opens mission menu).
		SetClickProxyForSite(Site, nullptr, &AcceptedMission.missionId);
	}
	else
	{
		if (bDebugLogs)
		{
			UE_LOG(LogTemp, Log,
			       TEXT("[DispatchWorldIndicators] OfferRemoved (Expired/Declined) | OfferId=%s | Site=%s"),
			       *OfferId.ToString(), *GetNameSafe(Site));
		}

		Site->SetOfferActive(false);

		// Clear click proxies since no offer and no mission.
		SetClickProxyForSite(Site, nullptr, nullptr);
	}

	offerToSite.Remove(OfferId);
}

void UDispatchMissionWorldIndicatorComponent::HandleMissionStateChanged(
	const FGuid& MissionId, EDispatchMissionState NewState)
{
	if (!missionManager.IsValid())
	{
		return;
	}

	ADispatchMissionSiteActor* Site = nullptr;

	if (TWeakObjectPtr<ADispatchMissionSiteActor>* Found = missionToSite.Find(MissionId))
	{
		Site = Found->IsValid() ? Found->Get() : nullptr;
	}

	if (!Site)
	{
		Site = ResolveSiteFromMission(MissionId);
		if (Site)
		{
			missionToSite.Add(MissionId, Site);
		}
	}

	if (bDebugLogs)
	{
		UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] MissionStateChanged | MissionId=%s | State=%d | Site=%s"),
		       *MissionId.ToString(), (int32)NewState, *GetNameSafe(Site));
	}

	if (!Site)
	{
		return;
	}

	Site->SetMissionState(NewState);

	// End states -> restore material now and clear click proxy.
	if (NewState == EDispatchMissionState::Completed || NewState == EDispatchMissionState::Failed || NewState ==
		EDispatchMissionState::Aborted)
	{
		Site->SetOfferActive(false);
		SetClickProxyForSite(Site, nullptr, nullptr);
		missionToSite.Remove(MissionId);
	}
}

#pragma endregion CALLBACKS
