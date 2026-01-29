#pragma once

#include "QuestObjective.generated.h"

class UQuestManagerComponent;

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
 * Base class for quest objectives
 */
UCLASS(Abstract, Blueprintable)
class QUESTSYSTEM_API UQuestObjective : public UObject
{
	GENERATED_BODY()

public:

	UQuestObjective();

	/** Initialize the objective */
	virtual void Initialize(UQuestManagerComponent* InQuestManager);

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

protected:

	/** Reference to quest manager */
	UPROPERTY()
	UQuestManagerComponent* QuestManager;

	/** Current state of the objective */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveState CurrentState;

	/** Objective description for UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText ObjectiveDescription;
};