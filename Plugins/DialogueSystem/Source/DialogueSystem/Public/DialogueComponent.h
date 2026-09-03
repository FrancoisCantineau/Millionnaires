// DialogueComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DialogueSystemTypes.h"
#include "DialogueComponent.generated.h"

class UDialogueDataAsset;
class UDialogueWidgetBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueEndedMulticast, UDialogueDataAsset*, EndedDialogue);

/**
 * Runs a UDialogueDataAsset. Place on the PlayerController (or wherever fits your interaction
 * flow). Decoupled from UI/audio by default — OnLineStarted/OnChoicesPresented/OnDialogueEnded
 * are yours to implement in Blueprint. Optionally, assign WidgetClass to have the component
 * create/update/destroy a UDialogueWidgetBase automatically instead (see that class).
 *
 * Anything can call StartDialogue — an NPC interaction, a trigger volume, a scripted event, an
 * incident, an Orchestrator action. This component doesn't know or care what triggered it.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DIALOGUESYSTEM_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueComponent();

	/** SpeakingActor is optional context for your own wiring (e.g. computing distance-to-player
	 *  to decide whether to relay onto speakers) — the plugin itself never reads it. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(UDialogueDataAsset* Dialogue, AActor* SpeakingActor = nullptr);

	/** Call when the current node is a branching node and the player picked one. Index refers to
	 *  the array most recently passed to OnChoicesPresented (already filtered by RequiredTags),
	 *  not necessarily the raw list authored on the node. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectChoice(int32 ChoiceIndex);

	/** Forces the current linear line to advance immediately instead of waiting out its natural
	 *  duration. No-op if waiting on a choice, or not currently in a dialogue. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SkipCurrentLine();

	/** Call from your OWN audio playback (e.g. an AudioComponent's OnAudioFinished) when a
	 *  line's VoiceLine has genuinely finished, for exact audio/subtitle sync. VoiceLine's
	 *  GetDuration() is only an estimate — accurate for a plain SoundWave, but can be wrong for
	 *  a SoundCue with randomization/looping nodes. If you never call this, a safety-margin
	 *  fallback timer (GetDuration() + a small buffer) still advances eventually, so a dialogue
	 *  can't get stuck waiting forever — just less precisely synced. No-op if not currently
	 *  waiting on a VoiceLine, or waiting on a choice. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void NotifyLineFinished();

	/** Ends the dialogue early (e.g. player walks away mid-conversation). Safe to call anytime. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsInDialogue() const { return CurrentDialogue != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	AActor* GetSpeakingActor() const { return SpeakingActor; }

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	UDialogueDataAsset* GetCurrentDialogue() const { return CurrentDialogue; }

	/** True once this dialogue has been started at least once (marked the instant it starts, not
	 *  only on natural completion — an interrupted dialogue still counts as "seen"). Only
	 *  meaningful for dialogues started OUTSIDE an Orchestrator-style sequence with its own
	 *  progress tracking. */
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool HasSeenDialogue(const UDialogueDataAsset* Dialogue) const;

	/** Read-only played-line log, capped at MaxHistoryEntries (oldest entries drop off). */
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	const TArray<FDialoguePlayedLine>& GetHistory() const { return History; }

	/** Self-contained save/restore — see FDialogueSaveData. Call these from your project's save
	 *  bridge (not from inside either plugin). */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FDialogueSaveData CaptureSaveData() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void RestoreSaveData(const FDialogueSaveData& SaveData);

	/** Tags describing the current game/world state, used to filter which choices are shown
	 *  (FDialogueChoice::RequiredTags). The plugin never populates this itself — your project
	 *  sets it (e.g. from WorldState/QuestSystem) before/while a dialogue is running. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FGameplayTagContainer CurrentContextTags;

	/** Oldest entries are dropped once History exceeds this, to bound memory over a long session. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	int32 MaxHistoryEntries = 200;

	/** Optional. If set, the component creates an instance of this widget in StartDialogue, adds
	 *  it to the viewport, forwards every line/choice/end event to it, and destroys it in
	 *  EndDialogue. Make a normal Widget Blueprint inheriting from your WidgetClass and implement
	 *  its 3 events like any other widget. Leave unset to handle UI entirely yourself via the
	 *  events below instead (both routes fire either way). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|UI")
	TSubclassOf<UDialogueWidgetBase> WidgetClass;

	/** Fired when a node's line starts playing — implement subtitle display + voice line playback. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnLineStarted(const FDialogueLine& Line);

	/** Fired when a branching node is reached, with only the choices whose RequiredTags currently
	 *  match CurrentContextTags. Implement choice UI. Call SelectChoice() in response. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnChoicesPresented(const TArray<FDialogueChoice>& Choices);

	/** Fired when a picked choice has non-empty TagsToGrantOnSelect — forward these to your own
	 *  state system (WorldState, QuestSystem, whatever). The plugin doesn't interpret them. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnChoiceTagsGranted(const FGameplayTagContainer& Tags);

	/** Fired the instant a node with a non-empty EventTag plays — for camera cuts, VFX, handing
	 *  off to an Orchestrator sequence, etc. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnNodeEventTriggered(FGameplayTag EventTag);

	/** Fired when the dialogue ends, whether naturally (NextNodeID == NAME_None, or every choice
	 *  filtered out) or via EndDialogue(). EndedDialogue is passed explicitly (rather than relying
	 *  on GetCurrentDialogue(), which is already nullptr by the time this fires) so a bridge
	 *  listening for completion can still tell which dialogue just ended. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueEnded(UDialogueDataAsset* EndedDialogue);

	/** Same information as OnDialogueEnded, as a real multicast delegate — use this instead when
	 *  an EXTERNAL object (not a Blueprint subclass of this component) needs to know completion,
	 *  e.g. a latent Orchestrator action waiting to call ActionRuntime->NotifyFinished(). */
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueEndedMulticast OnDialogueEndedDelegate;

private:
	UPROPERTY(Transient)
	TObjectPtr<UDialogueDataAsset> CurrentDialogue;

	UPROPERTY(Transient)
	TObjectPtr<AActor> SpeakingActor;

	UPROPERTY(Transient)
	TObjectPtr<UDialogueWidgetBase> ActiveWidget;

	FName CurrentNodeID;
	FTimerHandle AutoAdvanceTimerHandle;
	bool bWaitingOnChoice = false;
	bool bAwaitingExternalLineFinish = false;

	/** The exact (already-filtered) list last passed to OnChoicesPresented — SelectChoice indexes into this. */
	TArray<FDialogueChoice> PresentedChoices;

	TArray<FDialoguePlayedLine> History;

	/** Only meaningful for dialogues started outside any Orchestrator-tracked sequence. */
	TSet<FPrimaryAssetId> SeenDialogueIDs;

	void PlayNode(FName NodeID);
	void AutoAdvance();
};
