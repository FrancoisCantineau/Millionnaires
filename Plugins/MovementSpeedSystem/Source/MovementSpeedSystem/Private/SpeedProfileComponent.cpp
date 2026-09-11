// SpeedProfileComponent.cpp
#include "SpeedProfileComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USpeedProfileComponent::USpeedProfileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USpeedProfileComponent::SetSpeedProfile(FGameplayTag ProfileTag)
{
	if (!SpeedConfig || !SpeedConfig->SpeedProfiles.Contains(ProfileTag))
	{
		return false;
	}

	CurrentProfile = ProfileTag;
	RecalculateSpeed();
	return true;
}

void USpeedProfileComponent::AddSpeedModifier(FGameplayTag ModifierTag)
{
	ActiveModifiers.Add(ModifierTag);
	RecalculateSpeed();
}

void USpeedProfileComponent::RemoveSpeedModifier(FGameplayTag ModifierTag)
{
	ActiveModifiers.Remove(ModifierTag);
	RecalculateSpeed();
}

void USpeedProfileComponent::RecalculateSpeed()
{
	if (!SpeedConfig)
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
	{
		return;
	}

	const float* BaseSpeed = SpeedConfig->SpeedProfiles.Find(CurrentProfile);
	if (!BaseSpeed)
	{
		return;
	}

	float FinalSpeed = *BaseSpeed;

	// Multipliers stack multiplicatively across every active modifier.
	for (const FGameplayTag& Modifier : ActiveModifiers)
	{
		if (const float* Multiplier = SpeedConfig->SpeedMultipliers.Find(Modifier))
		{
			FinalSpeed *= *Multiplier;
		}
	}

	// Caps clamp down to the strictest (lowest) active limit.
	for (const FGameplayTag& Modifier : ActiveModifiers)
	{
		if (const float* Cap = SpeedConfig->SpeedCaps.Find(Modifier))
		{
			FinalSpeed = FMath::Min(FinalSpeed, *Cap);
		}
	}

	OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;
}
