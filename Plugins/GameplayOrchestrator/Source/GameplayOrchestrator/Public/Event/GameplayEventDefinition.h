#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Conditions/GameplayCondition.h"
#include "Sequence/GameplaySequence.h"
#include "GameplayEventDefinition.generated.h"

/**
 * How to handle a trigger of this definition, if an execution is already currently running.
 */
UENUM(BlueprintType)
enum class EGameplayExecutionPolicy : uint8
{
	/** Each trigger launch its own parallel execution (ex : footsteps). */
	AllowMultiple,
	/** Ignore this trigger, as long as an execution is still running (ex : door -> boss reveal, only one at a time) */
	IgnoreIfRunning,
	/** Cancels the current execution and restart from beginning (ex : canceled cinematic). */
	Restart,
	/** Queue this trigger, will be launched once the current execution is over (ex : script) */
	Queue
};

/**
 * Describes ONE gameplay event config : "whenever this EventTag arrives and these conditions are validated, launch this sequence."
 */
UCLASS(BlueprintType)
class GAMEPLAYORCHESTRATOR_API UGameplayEventDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Event tag that must be shared by the event bus, to start this definition's evaluation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event", meta = (MultiLine = true))
	FString Description;

	/**
	 * Conditions. A single validated group triggers the sequence. Empty array = valid.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event|Conditions")
	TArray<FGameplayConditionAndGroup> ConditionGroups;

	/**
	 * Sequence per intention (ex : "Boss Reveal", "Quest Progress", "Steam Acheivement" in separated sequences, rather than mixed up in one.
	 * Every valid sequence will execute (Array order).
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Event|Sequence")
	TArray<TObjectPtr<UGameplaySequence>> Sequences;

	/** If true, this definition will trigger only once per game session. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	bool bOneShot = false;

	/**
	 * Execution order, when multiple valid definitions answer to the same EventTag, in a single frame. (Higher = executed first).
	 * NB : doesn't exclude other definitions : by default, all definitions will execute, this only sets an order.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	int32 Priority = 0;

	/** Parallel definition execution logic for this one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	EGameplayExecutionPolicy ExecutionPolicy = EGameplayExecutionPolicy::AllowMultiple;

	/** Evaluates conditions. */
	bool EvaluateConditions(const FEventContext& Context) const
	{
		if (ConditionGroups.Num() == 0)
		{
			return true;
		}
		for (const FGameplayConditionAndGroup& Group : ConditionGroups)
		{
			if (Group.Evaluate(Context))
			{
				return true;
			}
		}
		return false;
	}
};
