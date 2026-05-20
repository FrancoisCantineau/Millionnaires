#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameEvent.generated.h"

USTRUCT(BlueprintType)
struct MILLIONNAIRES_API FGameEvent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FGameplayTag EventTag;

	UPROPERTY()
	FGameplayTagContainer SourceTags;

	UPROPERTY()
	UObject* Instigator = nullptr;
};