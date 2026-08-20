#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "QuestRuntimeState.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	NotStarted,
	Active,
	Completed,
	Failed
};

USTRUCT(BlueprintType)
struct FQuestObjectiveRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName ObjectiveId;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestObjectiveState State = EQuestObjectiveState::Inactive;

	/** Snapshot of the live instance's generic progress, kept in sync by whoever owns the objective.
	 *  -1/-1 if not applicable to this objective type. Flat data only - this struct replicates as
	 *  a whole, so it must never contain a UObject pointer (the live UQuestObjective instance lives
	 *  separately, server-only - see UQuestComponent::ServerOnlyLiveInstances). */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 ProgressCurrent = -1;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 ProgressTarget = -1;

	/** Snapshot of GetDisplayFields(), computed server-side where the live instance exists.
	 *  Flat text data, safe to replicate - covers anything beyond numeric progress (e.g. a timer)
	 *  without this struct needing a new field for every future objective type. */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestDisplayField> DisplayFields;

	/** Server world time (GetWorld()->GetTimeSeconds()) when this objective activated, if it has
	 *  a time limit. -1 if not applicable. Combined with the objective's own GetTimeLimitSeconds()
	 *  (static config, already known locally on the client from the same DataAsset), this lets a
	 *  client compute a live, accurate countdown every frame - no repeated replication needed. */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	float ActivationServerTime = -1.0f;
};

USTRUCT(BlueprintType)
struct FQuestRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName QuestId;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	EQuestState State = EQuestState::NotStarted;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TArray<FQuestObjectiveRuntimeState> Objectives;
};
