// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Psychic ability, causes the player sight to downgrade etc.
 */

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/AbilityBase.h"
#include "PsychicAbility.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UPsychicAbility : public UAbilityBase
{
	GENERATED_BODY()

public :

	virtual void ExecuteAbility_Implementation(AActor* Owner, AActor* Target, const UAbilityDataAsset* Data) override;
	
};
