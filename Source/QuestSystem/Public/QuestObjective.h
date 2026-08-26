#pragma once

#include "UObject/ScriptInterface.h"
#include "QuestObjectiveOwnerInterface.h"
#include "QuestObjective.generated.h"

/**
 * Generic label/value pair for anything an objective wants to show beyond its description
 * and numeric progress (e.g. a timer). Computed server-side (see GetDisplayFields below,
 * called only where the live UQuestObjective instance exists) and copied as plain text into
 * the replicated runtime state - the client never calls back into the objective itself.
 */
USTRUCT(BlueprintType)
struct FQuestDisplayField
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Label;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Value;
};

/**
 * Enum for quest objective states
 */
UENUM(BlueprintType)
enum class EQuestObjectiveState : uint8
{
	Inactive,
	Active,
	Completed,
	Failed
};

/**
 * Base class for quest objectives.
 * Talks only to IQuestObjectiveOwnerInterface - never a concrete manager/subsystem class,
 * so the same objective classes work no matter which UQuestComponent instance
 * (one per player, on their PlayerState) is driving them.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class QUESTSYSTEM_API UQuestObjective : public UObject
{
	GENERATED_BODY()

public:

	UQuestObjective();

	/** Initialize the objective. InOwner must implement IQuestObjectiveOwnerInterface. */
	virtual void Initialize(UObject* InOwner);

	/** Activate the objective */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	virtual void ObjectiveActivate();

	/** Complete the objective */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	virtual void ObjectiveComplete();

	/** Fail the objective */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	virtual void ObjectiveFail();

	/** Update objective logic */
	virtual void TickObjective(float DeltaTime);

	/** Get current state */
	UFUNCTION(BlueprintPure, Category = "Quest")
	EQuestObjectiveState GetState() const { return CurrentState; }

	/** Check if objective is active */
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsActive() const { return CurrentState == EQuestObjectiveState::Active; }

	/** Check if objective is completed */
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsCompleted() const { return CurrentState == EQuestObjectiveState::Completed; }

	/** Get objective description */
	UFUNCTION(BlueprintPure, Category = "Quest")
	FText GetObjectiveDescription() const { return ObjectiveDescription; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool HasTimeLimit() const { return bHasTimeLimit; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetTimeLimitSeconds() const { return TimeLimitSeconds; }

	/** Seconds left, or 0 if no time limit or the timer isn't running. */
	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetRemainingTime() const;

	/** Generic numeric progress, for objectives that have one (e.g. Counter's N/Target).
	 *  -1 means "not applicable" - base implementation, most objective types won't override this. */
	UFUNCTION(BlueprintPure, Category = "Quest")
	virtual int32 GetProgressCurrent() const { return -1; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	virtual int32 GetProgressTarget() const { return -1; }

	/** For restoring saved progress onto a freshly-activated instance (e.g. a loaded Counter
	 *  jumping straight to 2/5 instead of starting from 0). No-op on the base class - only
	 *  meaningful for objective types that track numeric progress. */
	virtual void SetProgressCurrent(int32 Value) {}

	/** Any extra label/value text this objective wants shown (e.g. time remaining). Only ever
	 *  called server-side, where the live instance actually exists - see UQuestComponent.
	 *  Override and call Super() first if a subclass wants to add its own fields on top. */
	virtual TArray<FQuestDisplayField> GetDisplayFields() const;

	/** Call this from a subclass whenever something display-relevant changes without the
	 *  objective completing or failing (e.g. Counter going from 1/3 to 2/3). Safe no-op if
	 *  there's no owner yet. */
	void NotifyProgressChanged();

	//~ UObject interface
	virtual UWorld* GetWorld() const override;
	//~ End UObject interface

protected:

	/** Owner driving this objective's lifecycle - a UQuestComponent on the owning player's PlayerState. */
	UPROPERTY()
	TScriptInterface<IQuestObjectiveOwnerInterface> Owner;

	/** Cached raw pointer to the same object as Owner, kept only to implement GetWorld(). */
	UPROPERTY()
	TWeakObjectPtr<UObject> OwnerObject;

	/** Current state of the objective */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveState CurrentState;

	/** Objective description for UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText ObjectiveDescription;

	/** Optional: any objective type can carry a time limit - fails automatically if it expires while Active.
	 *  Generic on purpose (not just for counting objectives): "reach the village in 60s", "hold the point
	 *  for 60s" need this just as much as "repair 5 things in 60s" does. */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	bool bHasTimeLimit = false;

	UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (EditCondition = "bHasTimeLimit", ClampMin = "0.1"))
	float TimeLimitSeconds = 60.0f;

private:

	void OnTimeLimitExpired();

	FTimerHandle TimeLimitHandle;
};
