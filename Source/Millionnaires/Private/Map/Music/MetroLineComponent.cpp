// MetroLineComponent.cpp
#include "MetroLineComponent.h"
#include "SplineFollowerComponent.h"
#include "MetroStation.h"
#include "Kismet/GameplayStatics.h"

UMetroLineComponent::UMetroLineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMetroLineComponent::BeginPlay()
{
	Super::BeginPlay();
	SubscribeToIncidentManager();
}

void UMetroLineComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnsubscribeFromIncidentManager();
}

void UMetroLineComponent::RegisterTrain(USplineFollowerComponent* Train)
{
	RegisteredTrains.AddUnique(Train);
}

void UMetroLineComponent::UnregisterTrain(USplineFollowerComponent* Train)
{
	RegisteredTrains.Remove(Train);
}

TArray<FMetroArrivalInfo> UMetroLineComponent::GetNextArrivals(AMetroStation* Station, int32 MaxCount) const
{
	TArray<FMetroArrivalInfo> Results;
	if (!Station)
	{
		return Results;
	}

	for (USplineFollowerComponent* Train : RegisteredTrains)
	{
		if (!Train)
		{
			continue;
		}

		const float ETA = Train->GetEstimatedTimeToStation(Station);
		if (ETA >= 0.f)
		{
			FMetroArrivalInfo Info;
			Info.Train = Train;
			Info.EstimatedSeconds = ETA;
			Results.Add(Info);
		}
	}

	Results.Sort([](const FMetroArrivalInfo& A, const FMetroArrivalInfo& B)
	{
		return A.EstimatedSeconds < B.EstimatedSeconds;
	});

	if (Results.Num() > MaxCount)
	{
		Results.SetNum(MaxCount);
	}

	return Results;
}

void UMetroLineComponent::SubscribeToIncidentManager()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GI))
	{
		return;
	}

	UIncidentManager* IM = GI->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM))
	{
		return;
	}

	IM->OnIncidentTriggered.AddDynamic(this, &UMetroLineComponent::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.AddDynamic(this, &UMetroLineComponent::OnIncidentResolvedDelegate);
}

void UMetroLineComponent::UnsubscribeFromIncidentManager()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GI))
	{
		return;
	}

	UIncidentManager* IM = GI->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM))
	{
		return;
	}

	IM->OnIncidentTriggered.RemoveDynamic(this, &UMetroLineComponent::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.RemoveDynamic(this, &UMetroLineComponent::OnIncidentResolvedDelegate);
}

void UMetroLineComponent::OnIncidentTriggeredDelegate(FShipIncident Incident)
{
	// NOTE: "Incident.AffectsSystem.Transit" is a new tag — add it alongside your existing
	// "Incident.AffectsSystem.Light" tag if it doesn't exist yet.
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Transit")))
	{
		return;
	}
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType)
	{
		return;
	}

	for (USplineFollowerComponent* Train : RegisteredTrains)
	{
		if (Train)
		{
			Train->SetHeldByIncident(true);
		}
	}
}

void UMetroLineComponent::OnIncidentResolvedDelegate(FShipIncident Incident)
{
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Transit")))
	{
		return;
	}
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType)
	{
		return;
	}

	for (USplineFollowerComponent* Train : RegisteredTrains)
	{
		if (Train)
		{
			Train->SetHeldByIncident(false);
		}
	}
}
