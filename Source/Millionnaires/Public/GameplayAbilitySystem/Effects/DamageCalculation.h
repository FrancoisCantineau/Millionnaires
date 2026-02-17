// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"
#include "DamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	
	UDamageCalculation();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;


protected:
	
	struct FDamageStatics
	{
		DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

		FDamageStatics()
		{
			DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Health, Target, false);
		}
	};
    
	static const FDamageStatics& DamageStatics()
	{
		static FDamageStatics Statics;
		return Statics;
	}
};
