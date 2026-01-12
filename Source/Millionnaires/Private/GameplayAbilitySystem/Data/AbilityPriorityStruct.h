#pragma once

#include "CoreMinimal.h"
#include "AbilityPriorityStruct.generated.h"

USTRUCT(BlueprintType)
struct FAbilityPriorityStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	int32 Priority;
};
