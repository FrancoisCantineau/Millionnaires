// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for abilities, used by the ennemies.
 */

#include "Characters/Ennemy/Ability/AbilityBase.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"


void UAbilityBase::ResetCooldown()
{
	bCanUseAbility = true;
}

void UAbilityBase::ExecuteAbility(AActor* Target)
{
}

bool UAbilityBase::UseAbility(AActor* Target)
{
	if (!CanUseAbility(Target))
		return false;
	
	UE_LOG(LogTemp, Log, TEXT("Ability used on %s"), *Target->GetName());

	ExecuteAbility(Target);
	
	// Cooldown
	bCanUseAbility = false;
	CurrentCooldown = MaxCooldown;

	if (MaxCooldown > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			this,
			&UAbilityBase::ResetCooldown,
			MaxCooldown,
			false
		);
	}
	else
	{
		ResetCooldown();
	}

	return true;
}

bool UAbilityBase::CanUseAbility(AActor* Target)
{
	if (!Target || !OwningCharacter)
		return false;

	if (!bCanUseAbility)
		return false;
	
	const float Distance =FVector::Dist(Target->GetActorLocation(), OwningCharacter->GetActorLocation());

	if (Distance < RangeMin || Distance > RangeMax)
		return false;

	return true;
}


