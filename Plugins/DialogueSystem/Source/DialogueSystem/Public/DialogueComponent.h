// DialogueComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueSystemTypes.h"
#include "DialogueComponent.generated.h"

class UDialogueDataAsset;

/**
 * Runs a UDialogueDataAsset. Place on the PlayerController (or wherever fits your interaction
 * flow). Completely decoupled from UI and audio playback — it only decides WHEN each line/choice
 * happens; OnLineStarted/OnChoicesPresented/OnDialogueEnded are yours to implement in Blueprint
 * (show subtitle UI, play the voice line wherever you want — through an AMusicSpeaker, directly
 * on the character, anything).
 *
 * Anything can call StartDialogue — an NPC interaction, a trigger volume, a scripted event, an
 * incident. This component doesn't know or care what triggered it.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class DIALOGUESYSTEM_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueComponent();

	/** SpeakingActor is optional context for your own wiring (e.g. computing distance-to-player
	 *  to decide whether to relay onto speakers) — the plugin itself never reads it. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(UDialogueDataAsset* Dialogue, AActor* SpeakingActor = nullptr);

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	AActor* GetSpeakingActor() const { return SpeakingActor; }

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	UDialogueDataAsset* GetCurrentDialogue() const { return CurrentDialogue; }

	/** Call when the current node is a branching node (Choices non-empty) and the player picked one. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectChoice(int32 ChoiceIndex);

	/** Ends the dialogue early (e.g. player walks away mid-conversation). Safe to call anytime. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsInDialogue() const { return CurrentDialogue != nullptr; }

	/** Fired when a node's line starts playing — implement subtitle display + voice line playback. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Dialogue")
	void OnLineStarted(const FDialogueLine& Line);

	/** Fired when a branching node is reached — implement choice UI. Call SelectChoice() in response. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnChoicesPresented(const TArray<FDialogueChoice>& Choices);

	/** Fired when the dialogue ends, whether naturally (NextNodeID == NAME_None) or via EndDialogue(). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueEnded();

private:
	UPROPERTY(Transient)
	TObjectPtr<UDialogueDataAsset> CurrentDialogue;

	UPROPERTY(Transient)
	TObjectPtr<AActor> SpeakingActor;

	FName CurrentNodeID;
	FTimerHandle AutoAdvanceTimerHandle;

	void PlayNode(FName NodeID);
	void AutoAdvance();
};
