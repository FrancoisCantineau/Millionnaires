// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Effects/GetCooldownValue.h"

#include "GameplayAbilitySystem/Abilities/Class/CostGameplayAbility.h"

float UGetCooldownValue::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UCostGameplayAbility* Ability = Cast<UCostGameplayAbility>(Spec.GetContext().GetAbility());

	if (!Ability)
	{
		return 0.1f;
	}

	return Ability->Cooldown;
}
