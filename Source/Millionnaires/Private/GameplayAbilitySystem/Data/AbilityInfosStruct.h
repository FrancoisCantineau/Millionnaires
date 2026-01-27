#pragma once

#include "CoreMinimal.h"
#include "AbilityInfosStruct.generated.h"

USTRUCT(BlueprintType)
struct FAbilityInfosStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float MinRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float MaxRange = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	int32 Priority;
};
