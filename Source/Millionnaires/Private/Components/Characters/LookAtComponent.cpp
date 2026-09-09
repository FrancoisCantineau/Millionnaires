// LookAtComponent.cpp
#include "Components/Characters/LookAtComponent.h"

ULookAtComponent::ULookAtComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULookAtComponent::SetLookAtTarget(FVector WorldLocation)
{
	DesiredTarget = WorldLocation;
	bWantsLookAt = true;
}

void ULookAtComponent::ClearLookAtTarget()
{
	bWantsLookAt = false;
}

void ULookAtComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float TargetAlpha = bWantsLookAt ? 1.f : 0.f;
	CurrentAlpha = FMath::FInterpConstantTo(CurrentAlpha, TargetAlpha, DeltaTime, AlphaBlendSpeed);

	// First frame ever: snap instead of easing from (0,0,0), or the head would visibly swing
	// from the world origin on the very first look.
	if (SmoothedTarget.IsZero() && !DesiredTarget.IsZero())
	{
		SmoothedTarget = DesiredTarget;
	}
	else
	{
		SmoothedTarget = FMath::VInterpTo(SmoothedTarget, DesiredTarget, DeltaTime, TargetLagSpeed);
	}
}