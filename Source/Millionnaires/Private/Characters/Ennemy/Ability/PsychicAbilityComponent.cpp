// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/PsychicAbilityComponent.h"

bool UPsychicAbilityComponent::UseAbility()
{

	if (!CanUseAbility() || !OwningCharacter)
	{
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "PsychicAbilityComponent");
	bCanUseAbility = false;
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			this,
			&UPsychicAbilityComponent::ResetCooldown,
			MaxCooldown,
			false
		);
	});
	return true;
}
