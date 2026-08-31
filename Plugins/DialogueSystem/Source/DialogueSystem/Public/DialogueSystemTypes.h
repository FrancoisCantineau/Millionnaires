// DialogueSystemTypes.h
#pragma once

#include "CoreMinimal.h"
#include "DialogueSystemTypes.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	/** Who is speaking, for subtitle display (e.g. "Marcus"). Leave empty for no speaker label. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText SpeakerName;

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
	 *  Non-empty makes this a branching node — playback waits for the player to pick one. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueChoice> Choices;

	/** Used only when Choices is empty. Leave as NAME_None to end the dialogue after this line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (EditCondition = "Choices.Num() == 0"))
	FName NextNodeID;
};
