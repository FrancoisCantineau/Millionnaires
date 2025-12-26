// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/Conditions/AbilityConditionBase.h"
#include "HealthTresholdCondition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (DisplayName = "Health Threshold"))
class MILLIONNAIRES_API UHealthTresholdCondition : public UAbilityConditionBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold", 
			 meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float MinHealthPercent = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold",
			  meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float MaxHealthPercent = 100.0f;
	
	virtual bool CheckCondition_Implementation(AActor* Owner) const override;

	float GetHealthPercent(AActor* Owner) const;
};
