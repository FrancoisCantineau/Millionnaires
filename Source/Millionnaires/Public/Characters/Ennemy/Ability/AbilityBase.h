// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for abilities, used by the ennemies.
 */

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "AbilityBase.generated.h"

class ABaseCharacter;


UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAbilityBase : public UObject
{
	GENERATED_BODY()


public:	

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void ExecuteAbility(AActor* Owner, AActor* Target, const class UAbilityDataAsset* Data);
    
	virtual void ExecuteAbility_Implementation(AActor* Owner, AActor* Target, const class UAbilityDataAsset* Data);
	
};
