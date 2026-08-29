// MetroWagon.cpp
#include "MetroWagon.h"
#include "Components/SplineComponent.h"

AMetroWagon::AMetroWagon()
{
	// No independent Tick — UpdateFollow() is driven explicitly by the head, in chain order.
	PrimaryActorTick.bCanEverTick = false;
}

float AMetroWagon::UpdateFollow(float DeltaTime, float PrecedingDistance, USplineComponent* Spline)
{
	if (!Spline)
	{
		return CurrentDistance;
	}

	const float SplineLength = Spline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return CurrentDistance;
	}

	float TargetDistance = PrecedingDistance - DesiredDistanceBehind;
	TargetDistance = FMath::Fmod(TargetDistance, SplineLength);
	if (TargetDistance < 0.f)
	{
		TargetDistance += SplineLength;
	}

	if (!bInitialized)
	{
		// Snap into place on the first frame rather than lagging in from a default of 0.
		CurrentDistance = TargetDistance;
		bInitialized = true;
	}
	else if (bUseElasticity)
	{
		// Shortest-path delta toward TargetDistance, accounting for spline wraparound
		// (otherwise FInterpTo could pull the wagon the "long way" around a looping line).
		float Delta = TargetDistance - CurrentDistance;
		if (FMath::Abs(Delta) > SplineLength * 0.5f)
		{
			Delta += (Delta > 0.f) ? -SplineLength : SplineLength;
		}
		CurrentDistance = FMath::FInterpTo(CurrentDistance, CurrentDistance + Delta, DeltaTime, FollowStiffness);
	}
	else
	{
		// Rigid: snap exactly to the target distance every frame.
		CurrentDistance = TargetDistance;
	}

	CurrentDistance = FMath::Fmod(CurrentDistance, SplineLength);
	if (CurrentDistance < 0.f)
	{
		CurrentDistance += SplineLength;
	}

	const FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	const FVector Tangent = Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
	const FRotator NewRotation = FRotationMatrix::MakeFromXZ(Tangent, FVector::UpVector).Rotator();

	// SetActorLocationAndRotation, not SetActorTransform — same scale-preservation reasoning as SplineFollowerComponent.
	SetActorLocationAndRotation(NewLocation, NewRotation);

	return CurrentDistance;
}