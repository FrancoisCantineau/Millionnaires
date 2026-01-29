// Fill out your copyright notice in the Description page of Project Settings.

/*
* Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "DamageCalculation" - Source
 * Notes: Helper to get custom cooldown value, based of the ability setting.
 */


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
