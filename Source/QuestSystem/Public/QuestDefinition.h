#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "QuestObjective.h"
#include "QuestDefinition.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Main,
	Side
};

/**
 * One objective slot within a quest definition.
 * Dependencies let the same flat array express linear, parallel, or branching
 * structures without committing to one shape up front:
 * - Linear: each entry depends on the previous one's ObjectiveId.
 * - Parallel: several entries with no dependencies between them.
 * - Branching: several entries all depending on the same parent ObjectiveId.
 */
USTRUCT(BlueprintType)
struct FQuestObjectiveEntry
{
	GENERATED_BODY()

	/** Unique (within this quest) identifier, referenced by other entries' Dependencies. */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FName ObjectiveId;

	/** The objective itself, configured inline (e.g. drop in a Counter, type 5 into its TargetCount right here -
	 *  no separate Blueprint subclass needed just to change a number). This is a template: at runtime, a fresh
	 *  duplicate of it is what actually runs, so re-starting the quest never reuses stale state. */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Quest")
	TObjectPtr<UQuestObjective> ObjectiveTemplate;

	/** ObjectiveIds that must be Completed before this objective activates. Empty = activates when the quest starts. */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TArray<FName> Dependencies;
};

/**
 * Static, designer-authored content for a quest. Never changes at runtime -
 * actual progression lives in FQuestRuntimeState instead.
 */
UCLASS(BlueprintType)
class QUESTSYSTEM_API UQuestDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FName QuestId;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FText Title;

	UPROPERTY(EditDefaultsOnly, Category = "Quest", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	EQuestType QuestType = EQuestType::Side;

	/** Optional: if set, the quest starts automatically when this tag is published on the GameplayEventBus.
	 *  Leave empty for quests only ever started explicitly (e.g. from an Orchestrator sequence). */
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FGameplayTag StartEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TArray<FQuestObjectiveEntry> Objectives;
};
