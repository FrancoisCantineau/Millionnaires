// DialogueSystemTypes.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialogueSystemTypes.generated.h"

class USoundBase;
class UDialogueSpeaker;

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	/** Who is speaking. Leave unset for a narrator/unattributed line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UDialogueSpeaker> Speaker;

	/** The subtitle text shown for this line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText LineText;

	/** Optional voice line. If set, its duration drives auto-advance timing for linear nodes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<USoundBase> VoiceLine;

	/** Fallback display duration (seconds) used only when VoiceLine is not set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "VoiceLine == nullptr"))
	float FallbackDuration = 3.f;
};

USTRUCT(BlueprintType)
struct FDialogueChoice
{
	GENERATED_BODY()

	/** Text shown to the player for this choice. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText ChoiceText;

	/** NodeID (in the same UDialogueDataAsset) this choice leads to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName NextNodeID;

	/** This choice is only shown if this query matches UDialogueComponent::CurrentContextTags at
	 *  the moment its node plays. Leave empty to always show it. The plugin never interprets
	 *  what these tags mean — your project populates CurrentContextTags from whatever state
	 *  system you use (WorldState, QuestSystem, anything). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Conditions")
	FGameplayTagQuery RequiredTags;

	/** Broadcast via UDialogueComponent::OnChoiceTagsGranted when this choice is picked. The
	 *  plugin doesn't act on these itself — bridge them to your state system in project code. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Conditions")
	FGameplayTagContainer TagsToGrantOnSelect;
};

USTRUCT(BlueprintType)
struct FDialogueNode
{
	GENERATED_BODY()

	/** Unique key for this node within its dialogue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName NodeID;

	/** The line spoken when this node plays. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FDialogueLine Line;

	/** Leave empty for a linear node (auto-advances to NextNodeID once Line finishes).
	 *  Non-empty makes this a branching node — playback waits for the player to pick one
	 *  (of whichever choices currently satisfy their RequiredTags). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueChoice> Choices;

	/** Used only when Choices is empty. Leave as NAME_None to end the dialogue after this line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "Choices.Num() == 0"))
	FName NextNodeID;

	/** Optional tag fired via UDialogueComponent::OnNodeEventTriggered the instant this node
	 *  plays — for a camera cut, VFX, or handing off to an Orchestrator sequence, without
	 *  needing a whole separate sequence just for a single narrative beat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FGameplayTag EventTag;

	/** Editor-only organizational note — not read at runtime. Helps navigating large dialogues. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = true))
	FString EditorComment;
};

/** One entry in a played-dialogue history/journal. */
USTRUCT(BlueprintType)
struct FDialoguePlayedLine
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueLine Line;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	double WorldTimeSeconds = 0.0;
};

/** Self-contained save contract for UDialogueComponent — no dependency on any specific save
 *  plugin, so DialogueSystem stays usable standalone. Bridge this to your actual save system in
 *  project code (the same way QuestSystem's FQuestSaveData is bridged), not inside either plugin. */
USTRUCT(BlueprintType)
struct FDialogueSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
	TArray<FPrimaryAssetId> SeenDialogues;

	UPROPERTY(BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialoguePlayedLine> History;
};
