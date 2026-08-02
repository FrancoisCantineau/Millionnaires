#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplaySequence.generated.h"

/**
 * A sequence step : a group of actions in parallel execution, they all start altogether.
 * Next step is reached, whenever they all are over, instantly for instant actions, or after NotifyFinished, for latent actions. 
 */

USTRUCT(BlueprintType)
struct GAMEPLAYORCHESTRATOR_API FGameplaySequenceStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Sequence Step")
	TArray<TObjectPtr<UGameplayActionBase>> ParallelActions;
};

/**
 * Actions sequence.
 */
UCLASS(EditInlineNew, DefaultToInstanced, BlueprintType)
class GAMEPLAYORCHESTRATOR_API UGameplaySequence : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	TArray<FGameplaySequenceStep> Steps;
};
