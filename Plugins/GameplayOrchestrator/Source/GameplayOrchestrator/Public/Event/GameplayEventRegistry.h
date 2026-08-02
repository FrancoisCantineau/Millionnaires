#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEventDefinition.h"
#include "GameplayEventRegistry.generated.h"

/**
 * Regroup a lot of GameplayEventDefinition, belonging to the same level/chapter (DA_HangarEvent contains Event_BossReveal, Event_LightsOut, Event_TrainArrival).
 *
 * Referenced by a UGameplayEventRegistryComponent, placed on an level actor,
 * which registers it to the UGameplayEventSubsystem when BeginPlay is called.
 */
UCLASS(BlueprintType)
class GAMEPLAYORCHESTRATOR_API UGameplayEventRegistry : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Registry")
	TArray<TObjectPtr<UGameplayEventDefinition>> Events;
};
