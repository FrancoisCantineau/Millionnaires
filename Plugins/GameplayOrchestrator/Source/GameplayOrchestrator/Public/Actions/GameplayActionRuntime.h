#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/GameplayEventContext.h"
#include "GameplayActionBase.h"
#include "GameplayActionRuntime.generated.h"

class UGameplaySequenceRuntime;
class UGameplayEventBus;

/**
 * Carry the execution state of a single latent action running. Created by
 * UGameplaySequenceRuntime, right before calling Action -> ExecuteLatent().
 * Destroyed once action is over (GC).
 *
 * Strict separation data/runtime : UGameplayAction (Instanced on an asset)
 * can be shared between several separate executions of a same sequence.
 * It must NEVER carry a FTimerHandle or FDelegateHandle by himself.
 * This object carries this state, on for each real execution.
 *
 * Gives generic fields ready to use, for most used latent cases (timer, bus sub),
 * to avoid each latent action to reinvent its own cleaning process. 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayActionRuntime : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UGameplayActionBase> Action;

	UPROPERTY()
	TWeakObjectPtr<UGameplaySequenceRuntime> OwningSequenceRuntime;

	/** Copied from SequenceRuntime creation. Allows a latent action to find its World/GameInstance */
	UPROPERTY()
	TWeakObjectPtr<UObject> WorldContextObject;

	/** Generic use for a delay type action. Cleaned automatically by Cancel() */
	FTimerHandle TimerHandle;

	/** Generic use for a "wait bus event" type action. Cleaned automatically by Cancel(). */
	FGameplayTag SubscribedEventTag;
	FDelegateHandle BusSubscriptionHandle;

	/** Must be called by the action itself when its latent execution is over (success, fail or completion). */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Action Runtime")
	void NotifyFinished(EGameplayActionResult Result);

	/**
	 * Called by the UGameplaySequenceRuntime::Cancel(), NEVER by the action itself.
	 * Cleans the timer/bus sub, calls the action's OnCacnelLatent hook for specific cleaning,
	 * then classify this execution as over (cancelled)
	 */
	void Cancel();

	UWorld* GetWorld() const override;

private:
	bool bFinished = false;
};
