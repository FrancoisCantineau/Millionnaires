#pragma once

#include "CoreMinimal.h"
#include "Core/GameplayEventContext.h"
#include "Actions/GameplayActionBase.h"
#include "GameplaySequenceRuntime.generated.h"

class UGameplaySequence;
class UGameplayActionRuntime;

UENUM(BlueprintType)
enum class ESequenceRuntimeState : uint8
{
	Running,
	Finished,
	Cancelled
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSequenceRuntimeFinished, class UGameplaySequenceRuntime*);

/**
 * Orchestrate ONE UGameplaySequence execution.
 * Carries 100% of state execution.
 *
 * Principle : Runtime doesn't know ANYTHING about actions details. It only knows how many steps must be terminated, for an action to be over.
 * However, it's the action's role to determines the exact timing of its ending, via ActionRuntime->NotifyFinished (immediatly for an instant action, later for a latent one).
 *
 * Possessed by UGameplayEventSubsystem (ActivateRuntimes), or UGameplayOrchestratorComponent, for local events, otherwise, GC could destroy it.
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplaySequenceRuntime : public UObject
{
	GENERATED_BODY()

public:
	/** Starts execution at step n°0. WorldContextObject allows to resolve World/GameInstance for latent actions */
	void Start(UGameplaySequence* InSequence, const FEventContext& InContext, UObject* InWorldContextObject);

	/**
	 * Cancels the sequence : all latent actions currently waiting will receive "Cancel()", then,
	 * state switches to Cancelled and OnFinished is broadcasted.
	 */
	UFUNCTION(BlueprintCallable, Category = "Gameplay Orchestrator|Sequence Runtime")
	void Cancel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay Orchestrator|Sequence Runtime")
	ESequenceRuntimeState GetState() const { return State; }

	/** Called by UGameplayActionRuntime::NotifyFinished, not designed to be called by extern use. */
	void OnActionRuntimeFinished(UGameplayActionRuntime* ActionRuntime, EGameplayActionResult Result);

	/** Broadcasted once, whether sequence ends naturally or by cancellation. Owner (Subsystem/Component) use it. */
	FOnSequenceRuntimeFinished OnFinished;

private:
	void ExecuteStep(int32 Index);
	void CheckStepCompletion();
	void FinishAs(ESequenceRuntimeState FinalState);

	UPROPERTY()
	TObjectPtr<UGameplaySequence> Sequence;

	UPROPERTY()
	TArray<TObjectPtr<UGameplayActionRuntime>> ActiveActionRuntimes;

	FEventContext Context;
	TWeakObjectPtr<UObject> WorldContextObject;

	int32 CurrentStepIndex = -1;
	int32 PendingActionsInCurrentStep = 0;
	ESequenceRuntimeState State = ESequenceRuntimeState::Running;
};
