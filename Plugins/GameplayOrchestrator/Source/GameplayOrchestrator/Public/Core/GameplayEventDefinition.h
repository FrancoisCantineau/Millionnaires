#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEventDefinition.generated.h"

class UGameplaySequence;

USTRUCT(BlueprintType)
struct FGameplayEventDefinition
{
	GENERATED_BODY()

public :

	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
	
	/*UPROPERTY(EditAnywhere)
	TArray<FTriggerConditionGroup> Conditions;*/

	UPROPERTY(EditAnywhere)
	bool bOneShot = true;


	UPROPERTY(EditAnywhere)
	int32 Priority = 0;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UGameplaySequence> Sequence;
	
};
 