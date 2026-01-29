// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "CostGameplayAbility" - Header
 * Notes: Custom ability class. Adding basic cooldown, cost etc. 
 */


#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySystem/Data/AbilityInfosStruct.h"
#include "CostGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UCostGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public :

	UPROPERTY(EditDefaultsOnly)
	float Cost;

	UPROPERTY(EditDefaultsOnly)
	float Cooldown;
	
};
