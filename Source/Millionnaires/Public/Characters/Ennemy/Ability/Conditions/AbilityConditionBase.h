// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbilityConditionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MILLIONNAIRES_API UAbilityConditionBase : public UObject
{
	GENERATED_BODY()

public :
	
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	bool CheckCondition(AActor* Owner) const;
	
	virtual bool CheckCondition_Implementation(AActor* Owner) const 
	{ 
		return true; 
	}
};
