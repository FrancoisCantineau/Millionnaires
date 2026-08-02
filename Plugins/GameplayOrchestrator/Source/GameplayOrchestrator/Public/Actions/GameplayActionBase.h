// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayActionBase.generated.h"

struct FEventContext;
class UGameplayActionRuntime;

/**
 * An instant action executes itself and ends within the same call (Print, SpawnActor, etc).
 * A latent action starts an execution which will end later (anim, timeline, waiting of an event). It needs a GameplayActionRuntime,
 * to carry its execution state.
 */

UENUM(BlueprintType)
enum class EGameplayActionExecutionType : uint8
{
	Instant,
	Latent
};

/**
 * Action result. Might alter the sequence behavior for the next actions to be executed. 
 */
UENUM(BlueprintType)
enum class EGameplayActionResult : uint8
{
	Succeeded,
	Failed,
	Cancelled
};


UCLASS(Abstract, Blueprintable, EditInlineNew)
class GAMEPLAYORCHESTRATOR_API UGameplayActionBase : public UObject
{
	GENERATED_BODY()

public:

	/** Instant by default. Latent action needs to override this to return latent */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Action")
	EGameplayActionExecutionType GetExecutionType();
	virtual EGameplayActionExecutionType GetExecutionType_Implementation() { return EGameplayActionExecutionType::Instant; }

	/** Called for instant actions. Must be ended before returning */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Action")
	void Execute(const FEventContext& Context);
	virtual void Execute_Implementation(const FEventContext& Context) {}

	/**
	 * Called for latent actions. Action HAS TO call ActionRuntime->NotifyFinished(Result) when execution is over
	 * (might be delayed with a timer, delegate or callback). As long as NotifyFinished is not called, step will remain
	 * in standby for this action
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay Orchestrator|Action")
	void ExecuteLatent(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context);
	virtual void ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context) {}

	/**
	 * Hook called by ActionRuntim::Cancel(), after the generic cleaning
	 * (timer, subscription bus/ gameplayactionRuntime). Overload only if your action needs a
	 * additionnal specific cleaning, such as : stop a timeline, cancel a move to...
	 * DO NOT CALL NotifyFinished here, ActionRuntime already handles it.
	 */
	virtual void OnCancelLatent(UGameplayActionRuntime* ActionRuntime) {}

#if WITH_EDITOR
	virtual FString GetEditorSummary() const { return GetClass()->GetName(); }
#endif
	
};
