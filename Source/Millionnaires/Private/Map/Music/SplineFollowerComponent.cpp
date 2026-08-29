// SplineFollowerComponent.cpp
#include "SplineFollowerComponent.h"
#include "Components/SplineComponent.h"
#include "Curves/CurveFloat.h"
#include "MetroStation.h"
#include "MetroWagon.h"
#include "MetroLineComponent.h"

USplineFollowerComponent::USplineFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USplineFollowerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!SplineOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineFollowerComponent on %s has no SplineOwner assigned."), *GetOwner()->GetName());
		return;
	}

	TargetSpline = SplineOwner->FindComponentByClass<USplineComponent>();
	if (!TargetSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineOwner %s has no USplineComponent."), *SplineOwner->GetName());
		return;
	}

	if (!bLoopsContinuously)
	{
		DiscoverAndSortStations();
	}

	const float SplineLength = TargetSpline->GetSplineLength();
	if (SplineLength > 0.f)
	{
		DistanceAlongSpline = FMath::Fmod(InitialDistanceAlongSpline, SplineLength);
		if (DistanceAlongSpline < 0.f)
		{
			DistanceAlongSpline += SplineLength;
		}
	}

	if (!bLoopsContinuously && Stations.Num() > 0)
	{
		// Find the first station ahead of our starting position, wrapping to Stations[0]
		// if every station is "behind" us (i.e. we start past the last one on the loop).
		NextStationIndex = 0;
		for (int32 i = 0; i < Stations.Num(); ++i)
		{
			if (Stations[i] && Stations[i]->DistanceAlongSpline > DistanceAlongSpline)
			{
				NextStationIndex = i;
				break;
			}
		}
	}

	CachedLineComponent = SplineOwner->FindComponentByClass<UMetroLineComponent>();
	if (CachedLineComponent)
	{
		CachedLineComponent->RegisterTrain(this);
	}

	StartMoving();
}

void USplineFollowerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CachedLineComponent)
	{
		CachedLineComponent->UnregisterTrain(this);
	}
	Super::EndPlay(EndPlayReason);
}

void USplineFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsMoving || !TargetSpline)
	{
		return;
	}

	AdvanceAlongSpline(DeltaTime);
}

void USplineFollowerComponent::SetHeldByIncident(bool bHeld)
{
	bHeldByIncident = bHeld;

	if (bHeld)
	{
		// If currently rolling (anywhere on the spline, not necessarily near a station), brake
		// immediately right where we are. If already stopped at a station, the existing
		// bHeldByIncident check in the Dwelling case handles it — nothing more to do here.
		if (State == ESplineFollowerState::Accelerating
			|| State == ESplineFollowerState::Cruising
			|| State == ESplineFollowerState::Braking)
		{
			State = ESplineFollowerState::EmergencyBraking;
		}
	}
	else if (State == ESplineFollowerState::EmergencyStopped)
	{
		OnEmergencyResume();
		for (AMetroWagon* Wagon : Wagons)
		{
			if (Wagon)
			{
				Wagon->OnDoorsShouldClose();
			}
		}
		State = ESplineFollowerState::Accelerating;
	}
	// If currently held in Dwelling at a station, UpdateStateMachine's Dwelling case will
	// naturally proceed to DoorsClosing on its own next Tick now that bHeldByIncident is false.
}

void USplineFollowerComponent::StartMoving()
{
	bIsMoving = true;
	// If we were fully halted (StopMoving was called) while cruising/accelerating/braking with
	// no active door phase, resume motion. Door phases resume on their own via their timers —
	// nothing to force here while DoorsOpening/Dwelling/DoorsClosing.
	if (CurrentSpeed <= 0.f
		&& State != ESplineFollowerState::DoorsOpening
		&& State != ESplineFollowerState::Dwelling
		&& State != ESplineFollowerState::DoorsClosing)
	{
		State = bLoopsContinuously ? ESplineFollowerState::Cruising : ESplineFollowerState::Accelerating;
	}
}

void USplineFollowerComponent::StopMoving()
{
	bIsMoving = false;
}

void USplineFollowerComponent::DiscoverAndSortStations()
{
	if (Stations.Num() == 0 && SplineOwner)
	{
		// Auto-discover: any AMetroStation actor attached as a child of SplineOwner.
		TArray<AActor*> AttachedActors;
		SplineOwner->GetAttachedActors(AttachedActors);
		for (AActor* Attached : AttachedActors)
		{
			if (AMetroStation* Station = Cast<AMetroStation>(Attached))
			{
				Stations.Add(Station);
			}
		}
	}

	// Project each station onto the spline and sort by resulting distance.
	for (AMetroStation* Station : Stations)
	{
		if (Station)
		{
			const float Key = TargetSpline->FindInputKeyClosestToWorldLocation(Station->GetActorLocation());
			Station->DistanceAlongSpline = TargetSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
		}
	}

	Stations.Sort([](const TObjectPtr<AMetroStation>& A, const TObjectPtr<AMetroStation>& B)
	{
		return A->DistanceAlongSpline < B->DistanceAlongSpline;
	});
}

float USplineFollowerComponent::GetDistanceToNextStation(float SplineLength) const
{
	if (Stations.Num() == 0 || !Stations.IsValidIndex(NextStationIndex) || !Stations[NextStationIndex])
	{
		return TNumericLimits<float>::Max();
	}

	const float StationDistance = Stations[NextStationIndex]->DistanceAlongSpline;
	float Delta = StationDistance - DistanceAlongSpline;
	if (Delta < 0.f)
	{
		// Station is "behind" our raw distance value — wrap around a looping spline.
		Delta += SplineLength;
	}
	return Delta;
}

void USplineFollowerComponent::UpdateStateMachine(float DeltaTime, float SplineLength)
{
	switch (State)
	{
		case ESplineFollowerState::Accelerating:
		{
			CurrentSpeed = FMath::Min(CurrentSpeed + AccelRate * DeltaTime, CruiseSpeed);
			if (CurrentSpeed >= CruiseSpeed)
			{
				State = ESplineFollowerState::Cruising;
			}
			// Fall through to check if we're already within brake distance of a very close station.
		}
		// intentional fallthrough
		case ESplineFollowerState::Cruising:
		{
			const float DistanceToStation = GetDistanceToNextStation(SplineLength);
			if (DistanceToStation <= BrakeDistance)
			{
				State = ESplineFollowerState::Braking;
			}
			break;
		}
		case ESplineFollowerState::Braking:
		{
			const float DistanceToStation = GetDistanceToNextStation(SplineLength);

			if (SpeedCurve)
			{
				const float Normalized = FMath::Clamp(DistanceToStation / BrakeDistance, 0.f, 1.f);
				CurrentSpeed = CruiseSpeed * SpeedCurve->GetFloatValue(Normalized);
			}
			else
			{
				// Speed derived directly from remaining distance (v = sqrt(2 * a * d)) so
				// CurrentSpeed and DistanceToStation always reach zero together — avoids
				// speed hitting zero (or near it) while distance is still large.
				CurrentSpeed = FMath::Min(CurrentSpeed, FMath::Sqrt(FMath::Max(0.f, 2.f * BrakeRate * DistanceToStation)));
			}

			// Arrive when this frame's movement would reach or pass the station.
			const float ArrivalThreshold = FMath::Max(CurrentSpeed * DeltaTime, 1.f);
			if (DistanceToStation <= ArrivalThreshold)
			{
				// Snap exactly onto the station to avoid over/undershoot accumulating error.
				DistanceAlongSpline = Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex]->DistanceAlongSpline : DistanceAlongSpline;
				CurrentSpeed = 0.f;
				State = ESplineFollowerState::DoorsOpening;
				PhaseTimeRemaining = DoorOpenDuration;
				AMetroStation* ArrivedStation = Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex].Get() : nullptr;
				OnArrivedAtStation(ArrivedStation);
				for (AMetroWagon* Wagon : Wagons)
				{
					if (Wagon)
					{
						Wagon->OnDoorsShouldOpen();
					}
				}
			}
			break;
		}
		case ESplineFollowerState::DoorsOpening:
		{
			PhaseTimeRemaining -= DeltaTime;
			if (PhaseTimeRemaining <= 0.f)
			{
				State = ESplineFollowerState::Dwelling;
				PhaseTimeRemaining = Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex]->DwellTime : 8.f;
			}
			break;
		}
		case ESplineFollowerState::Dwelling:
		{
			if (bHeldByIncident)
			{
				// Doors stay open, dwell timer does not progress, until the incident resolves.
				// (If an incident triggers exactly during DoorsClosing instead, that specific
				// train will still depart this cycle and get held at the following station.)
				break;
			}
			PhaseTimeRemaining -= DeltaTime;
			if (PhaseTimeRemaining <= 0.f && bIsMoving)
			{
				State = ESplineFollowerState::DoorsClosing;
				PhaseTimeRemaining = DoorCloseDuration;
				AMetroStation* DepartingStation = Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex].Get() : nullptr;
				OnDepartingStation(DepartingStation);
				for (AMetroWagon* Wagon : Wagons)
				{
					if (Wagon)
					{
						Wagon->OnDoorsShouldClose();
					}
				}
			}
			break;
		}
		case ESplineFollowerState::DoorsClosing:
		{
			PhaseTimeRemaining -= DeltaTime;
			if (PhaseTimeRemaining <= 0.f)
			{
				NextStationIndex = Stations.Num() > 0 ? (NextStationIndex + 1) % Stations.Num() : 0;
				State = ESplineFollowerState::Accelerating;
			}
			break;
		}
		case ESplineFollowerState::EmergencyBraking:
		{
			// Decelerate wherever we happen to be — no target distance/station involved, unlike
			// the normal Braking case, since this can trigger anywhere on the spline.
			CurrentSpeed = FMath::Max(CurrentSpeed - BrakeRate * DeltaTime, 0.f);
			if (CurrentSpeed <= KINDA_SMALL_NUMBER)
			{
				CurrentSpeed = 0.f;
				State = ESplineFollowerState::EmergencyStopped;
				OnEmergencyStop();
				for (AMetroWagon* Wagon : Wagons)
				{
					if (Wagon)
					{
						Wagon->OnDoorsShouldOpen();
					}
				}
			}
			break;
		}
		case ESplineFollowerState::EmergencyStopped:
		{
			// Nothing to do here — SetHeldByIncident(false) handles resuming from this state.
			break;
		}
	}
}

void USplineFollowerComponent::AdvanceAlongSpline(float DeltaTime)
{
	const float SplineLength = TargetSpline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return;
	}

	if (bLoopsContinuously)
	{
		CurrentSpeed = CruiseSpeed;
	}
	else
	{
		const ESplineFollowerState PreviousState = State;
		UpdateStateMachine(DeltaTime, SplineLength);
		if (State != PreviousState)
		{
			OnStateChanged(PreviousState, State);
		}
	}

	DistanceAlongSpline += CurrentSpeed * DeltaTime;
	DistanceAlongSpline = FMath::Fmod(DistanceAlongSpline, SplineLength);
	if (DistanceAlongSpline < 0.f)
	{
		DistanceAlongSpline += SplineLength;
	}

	const FVector NewLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	const FVector Tangent = TargetSpline->GetDirectionAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	// Built from the tangent + a fixed world-up vector rather than the spline's own interpolated
	// point rotation — the latter carries each point's authored roll, which drifts over distance
	// if points weren't all given exactly consistent roll values.
	const FRotator NewRotation = FRotationMatrix::MakeFromXZ(Tangent, FVector::UpVector).Rotator();

	// SetActorLocationAndRotation (not SetActorTransform) so we never touch the Actor's own Scale —
	// SetActorTransform would silently reset it to (1,1,1), since GetTransformAtDistanceAlongSpline's
	// bUseScale=false forces the scale component of the returned transform to (1,1,1).
	GetOwner()->SetActorLocationAndRotation(NewLocation, NewRotation);

	// Drive the wagon chain in order (head -> wagon 1 -> wagon 2 -> ...), each wagon reading the
	// already-updated distance of the element in front of it this same frame.
	float PrecedingDistance = DistanceAlongSpline;
	for (AMetroWagon* Wagon : Wagons)
	{
		if (Wagon)
		{
			PrecedingDistance = Wagon->UpdateFollow(DeltaTime, PrecedingDistance, TargetSpline);
		}
	}
}

float USplineFollowerComponent::GetEstimatedTimeToStation(AMetroStation* Station) const
{
	if (!Station || Stations.Num() == 0 || !TargetSpline)
	{
		return -1.f;
	}

	const int32 TargetIndex = Stations.IndexOfByKey(Station);
	if (TargetIndex == INDEX_NONE)
	{
		return -1.f;
	}

	const float SplineLength = TargetSpline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return -1.f;
	}

	const float SafeCruiseSpeed = FMath::Max(CruiseSpeed, 1.f);
	const float DoorOverhead = DoorOpenDuration + DoorCloseDuration;

	float TotalTime = 0.f;

	if (State == ESplineFollowerState::DoorsOpening || State == ESplineFollowerState::Dwelling || State == ESplineFollowerState::DoorsClosing)
	{
		// Currently stopped at Stations[NextStationIndex].
		if (NextStationIndex == TargetIndex)
		{
			return 0.f;
		}

		TotalTime += PhaseTimeRemaining;
		if (State == ESplineFollowerState::DoorsOpening)
		{
			TotalTime += (Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex]->DwellTime : 0.f) + DoorCloseDuration;
		}
		else if (State == ESplineFollowerState::Dwelling)
		{
			TotalTime += DoorCloseDuration;
		}
		// DoorsClosing: PhaseTimeRemaining already covers the rest of this stop.
	}
	else
	{
		// Accelerating/Cruising/Braking — approximate remaining time on the current leg.
		TotalTime += GetDistanceToNextStation(SplineLength) / SafeCruiseSpeed;

		if (NextStationIndex == TargetIndex)
		{
			return TotalTime;
		}

		TotalTime += (Stations.IsValidIndex(NextStationIndex) ? Stations[NextStationIndex]->DwellTime : 0.f) + DoorOverhead;
	}

	// Walk forward through intermediate stations until reaching TargetIndex.
	int32 Index = NextStationIndex;
	while (Index != TargetIndex)
	{
		const int32 NextIndex = (Index + 1) % Stations.Num();
		if (!Stations.IsValidIndex(NextIndex) || !Stations.IsValidIndex(Index))
		{
			return -1.f;
		}

		float LegDistance = Stations[NextIndex]->DistanceAlongSpline - Stations[Index]->DistanceAlongSpline;
		LegDistance = FMath::Fmod(LegDistance + SplineLength, SplineLength);
		TotalTime += LegDistance / SafeCruiseSpeed;

		if (NextIndex != TargetIndex)
		{
			TotalTime += Stations[NextIndex]->DwellTime + DoorOverhead;
		}

		Index = NextIndex;
	}

	return TotalTime;
}
