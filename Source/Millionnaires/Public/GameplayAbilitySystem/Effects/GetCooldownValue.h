// Fill out your copyright notice in the Description page of Project Settings.

/*
* Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "DamageCalculation" - Header
 * Notes: Helper to get custom cooldown value, based of the ability setting.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GetCooldownValue.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UGetCooldownValue : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
