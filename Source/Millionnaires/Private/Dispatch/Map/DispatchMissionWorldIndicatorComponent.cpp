/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionWorldIndicatorComponent" - Source
 * Notes: Binds to DispatchMissionManagerComponent and drives world indicators (site highlight).
 */
#include "Dispatch/Map/DispatchMissionWorldIndicatorComponent.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Map/DispatchMissionSiteActor.h"
#include "Dispatch/Missions/DispatchMissionDefinition.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/UnrealType.h"

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
                if (UDispatchMissionManagerComponent* Found = It->FindComponentByClass<UDispatchMissionManagerComponent>())
                {
                    missionManager = Found;
                    break;
                }
            }
        }
    }

    if (!missionManager.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchWorldIndicators] MissionManager not found. Add this component on the SAME Owner as UDispatchMissionManagerComponent (recommended: Dispatch PlayerController)."));
        return;
    }

    missionManager->OnOfferAdded.AddDynamic(this, &UDispatchMissionWorldIndicatorComponent::HandleOfferAdded);
    missionManager->OnOfferRemoved.AddDynamic(this, &UDispatchMissionWorldIndicatorComponent::HandleOfferRemoved);
    missionManager->OnMissionStateChanged.AddDynamic(this, &UDispatchMissionWorldIndicatorComponent::HandleMissionStateChanged);

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] Bound | Owner=%s | MissionManagerOwner=%s"),
            *GetNameSafe(GetOwner()), *GetNameSafe(missionManager->GetOwner()));
    }
}

bool UDispatchMissionWorldIndicatorComponent::TryReadOptionalOfferLocationActor(const UScriptStruct* OfferStruct, const void* OfferPtr, ADispatchMissionSiteActor*& OutSite) const
{
    OutSite = nullptr;
    if (!OfferStruct || !OfferPtr)
    {
        return false;
    }

    // If your FDispatchMissionOffer struct contains a UPROPERTY named "locationActor", we can read it even if our code doesn't depend on that member.
    FObjectPropertyBase* ObjProp = FindFProperty<FObjectPropertyBase>(OfferStruct, TEXT("locationActor"));
    if (!ObjProp)
    {
        return false;
    }

    UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(OfferPtr);
    OutSite = Cast<ADispatchMissionSiteActor>(Obj);
    return (OutSite != nullptr);
}

bool UDispatchMissionWorldIndicatorComponent::TryReadOptionalMissionLocationValue(const UObject* Obj, const FName PropertyName, int64& OutValue) const
{
    OutValue = 0;
    if (!Obj)
    {
        return false;
    }

    FProperty* Prop = Obj->GetClass()->FindPropertyByName(PropertyName);
    if (!Prop)
    {
        return false;
    }

    const void* Container = Obj;

    if (const FEnumProperty* EnumProp = CastField<FEnumProperty>(Prop))
    {
        const FNumericProperty* Underlying = EnumProp->GetUnderlyingProperty();
        OutValue = Underlying ? Underlying->GetSignedIntPropertyValue(EnumProp->ContainerPtrToValuePtr<void>(Container)) : 0;
        return true;
    }

    if (const FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
    {
        OutValue = (int64)ByteProp->GetPropertyValue_InContainer(Container);
        return true;
    }

    if (const FIntProperty* IntProp = CastField<FIntProperty>(Prop))
    {
        OutValue = (int64)IntProp->GetPropertyValue_InContainer(Container);
        return true;
    }

    return false;
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::PickBestSite(const FVector& WorldLocation, const int64* DesiredLocationOrNull, int32 Seed) const
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

    // If we have a desired location enum value, try to pick a matching site first (using EDispatchMissionLocation).
    if (DesiredLocationOrNull)
    {
        const EDispatchMissionLocation Desired = (EDispatchMissionLocation)(*DesiredLocationOrNull);

        // Exact matches (only if not Any).
        if (Desired != EDispatchMissionLocation::Any)
        {
            TArray<ADispatchMissionSiteActor*> Exact;
            for (ADispatchMissionSiteActor* S : AllSites)
            {
                if (S && S->GetMissionLocation() == Desired)
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

        // If desired is Any OR no exact matches: allow Any sites as fallback.
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
    }

    // Fallback: nearest to world location.
    ADispatchMissionSiteActor* Best = AllSites[0];
    double BestDist2 = FVector::DistSquared(AllSites[0]->GetActorLocation(), WorldLocation);

    for (ADispatchMissionSiteActor* S : AllSites)
    {
        if (!S) continue;
        const double D2 = FVector::DistSquared(S->GetActorLocation(), WorldLocation);
        if (D2 < BestDist2)
        {
            BestDist2 = D2;
            Best = S;
        }
    }

    return Best;
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::ResolveSiteFromOffer(const FGuid& OfferId)
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

    // 1) Optional direct locationActor on the Offer struct (via reflection).
    ADispatchMissionSiteActor* DirectSite = nullptr;
    if (TryReadOptionalOfferLocationActor(FDispatchMissionOffer::StaticStruct(), &Offer, DirectSite))
    {
        return DirectSite;
    }

    // 2) Optional missionLocation on Offer or Definition (via reflection).
    int64 LocationValue = 0;
    const bool bHasLocationOnOffer = TryReadOptionalMissionLocationValue(reinterpret_cast<const UObject*>(&Offer), TEXT("missionLocation"), LocationValue);
    const bool bHasLocationOnDef = (!bHasLocationOnOffer && Offer.definition) ? TryReadOptionalMissionLocationValue(Offer.definition, TEXT("missionLocation"), LocationValue) : false;
    const int64* DesiredLocPtr = (bHasLocationOnOffer || bHasLocationOnDef) ? &LocationValue : nullptr;

    // 3) Pick best site.
    return PickBestSite(Offer.worldLocation, DesiredLocPtr, Offer.seed);
}

ADispatchMissionSiteActor* UDispatchMissionWorldIndicatorComponent::ResolveSiteFromMission(const FGuid& MissionId)
{
    if (!missionManager.IsValid())
    {
        return nullptr;
    }

    const TArray<FDispatchActiveMission> Missions = missionManager->GetActiveMissions();
    const FDispatchActiveMission* Found = nullptr;

    for (const FDispatchActiveMission& M : Missions)
    {
        if (M.missionId == MissionId)
        {
            Found = &M;
            break;
        }
    }

    if (!Found)
    {
        return nullptr;
    }

    // 1) If we still have the offer mapping, use it.
    if (const TWeakObjectPtr<ADispatchMissionSiteActor>* FromOffer = offerToSite.Find(Found->sourceOfferId))
    {
        if (FromOffer && FromOffer->IsValid())
        {
            return FromOffer->Get();
        }
    }

    // 2) Optional missionLocation on Definition (via reflection).
    int64 LocationValue = 0;
    const bool bHasLocationOnDef =
        Found->definition ? TryReadOptionalMissionLocationValue(Found->definition.Get(), TEXT("missionLocation"), LocationValue) : false;
    const int64* DesiredLocPtr = bHasLocationOnDef ? &LocationValue : nullptr;

    // 3) Pick best site.
    return PickBestSite(Found->worldLocation, DesiredLocPtr, Found->seed);
}

#pragma endregion INTERNAL

#pragma region CALLBACKS

void UDispatchMissionWorldIndicatorComponent::HandleOfferAdded(const FGuid& OfferId)
{
    if (!missionManager.IsValid())
    {
        BindToMissionManager();
    }

    ADispatchMissionSiteActor* Site = ResolveSiteFromOffer(OfferId);

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] OfferAdded | OfferId=%s | Site=%s"),
            *OfferId.ToString(), *GetNameSafe(Site));
    }

    if (!Site)
    {
        return;
    }

    Site->RebuildMeshCache();
    Site->SetOfferActive(true);

    offerToSite.Add(OfferId, Site);
}

void UDispatchMissionWorldIndicatorComponent::HandleOfferRemoved(const FGuid& OfferId)
{
    if (!missionManager.IsValid())
    {
        return;
    }

    TWeakObjectPtr<ADispatchMissionSiteActor>* FoundSitePtr = offerToSite.Find(OfferId);
    if (!FoundSitePtr || !FoundSitePtr->IsValid())
    {
        offerToSite.Remove(OfferId);
        return;
    }

    ADispatchMissionSiteActor* Site = FoundSitePtr->Get();

    // If an active mission exists with this sourceOfferId, the offer was accepted.
    const TArray<FDispatchActiveMission> Missions = missionManager->GetActiveMissions();
    const FDispatchActiveMission* AcceptedMission = nullptr;

    for (const FDispatchActiveMission& M : Missions)
    {
        if (M.sourceOfferId == OfferId)
        {
            AcceptedMission = &M;
            break;
        }
    }

    if (AcceptedMission)
    {
        missionToSite.Add(AcceptedMission->missionId, Site);

        if (bDebugLogs)
        {
            UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] OfferRemoved (Accepted) | OfferId=%s -> MissionId=%s | Site=%s"),
                *OfferId.ToString(), *AcceptedMission->missionId.ToString(), *GetNameSafe(Site));
        }

        // Keep highlight alive; mission state updates will drive it.
        Site->SetMissionState(AcceptedMission->state);
    }
    else
    {
        if (bDebugLogs)
        {
            UE_LOG(LogTemp, Log, TEXT("[DispatchWorldIndicators] OfferRemoved (Expired/Declined) | OfferId=%s | Site=%s"),
                *OfferId.ToString(), *GetNameSafe(Site));
        }

        Site->SetOfferActive(false);
    }

    offerToSite.Remove(OfferId);
}

void UDispatchMissionWorldIndicatorComponent::HandleMissionStateChanged(const FGuid& MissionId, EDispatchMissionState NewState)
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

    // End states -> restore material now.
    if (NewState == EDispatchMissionState::Completed || NewState == EDispatchMissionState::Failed || NewState == EDispatchMissionState::Aborted)
    {
        Site->SetOfferActive(false);
        missionToSite.Remove(MissionId);
    }
}

#pragma endregion CALLBACKS
