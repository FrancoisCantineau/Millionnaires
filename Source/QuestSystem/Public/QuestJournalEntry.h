#pragma once

#include "CoreMinimal.h"
#include "QuestRuntimeState.h"
#include "QuestDefinition.h"
#include "QuestJournalEntry.generated.h"

USTRUCT(BlueprintType)
struct FQuestJournalObjectiveEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName ObjectiveId;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveState State = EQuestObjectiveState::Inactive;

	/** True if ProgressCurrent/ProgressTarget are meaningful (e.g. a Counter) - false for objective
	 *  types with no numeric progress, so the widget knows whether to show a "3/5" style label at all. */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	bool bHasProgress = false;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 ProgressCurrent = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 ProgressTarget = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestDisplayField> DisplayFields;

	/** True if this objective has a time limit at all - check before using the two fields below. */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	bool bHasTimeLimit = false;

	/** Server world time when the timer started. Combine with TimeLimitSeconds and the current
	 *  local GetWorld()->GetTimeSeconds() to compute a live countdown: Remaining = TimeLimitSeconds -
	 *  (GetWorld()->GetTimeSeconds() - ActivationServerTime). Do this in a widget Tick, not here -
	 *  this struct is only rebuilt when the quest state actually changes, not every frame. */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	float ActivationServerTime = -1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	float TimeLimitSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct FQuestJournalEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName QuestId;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Title;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestType QuestType = EQuestType::Side;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestState State = EQuestState::NotStarted;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestJournalObjectiveEntry> Objectives;
};
