#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_StartDialogue.generated.h"

class UDialogueDataAsset;
class UDialogueComponent;

/**
 * Bridge action - connects GameplayOrchestrator to DialogueSystem. Lives here (in the game
 * module), never inside either plugin: it's the only piece of code in the project that needs
 * to know both UGameplayActionBase and UDialogueComponent at once.
 *
 * Latent (not Instant): the sequence step stays in standby for as long as the dialogue is
 * running, and only resumes once the player finishes/ends it — via UDialogueComponent's
 * OnDialogueEndedDelegate, the multicast-delegate companion to its BlueprintImplementableEvent
 * (needed here specifically because this is an external object, not a Blueprint subclass of the
 * component, so it can't just override the event).
 */
UCLASS(meta = (DisplayName = "Start Dialogue"))
class MILLIONNAIRES_API UGameplayAction_StartDialogue : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UDialogueDataAsset> DialogueData;

	virtual EGameplayActionExecutionType GetExecutionType_Implementation() override;
	virtual void ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context) override;
	virtual void OnCancelLatent(UGameplayActionRuntime* ActionRuntime) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override;
#endif

private:
	UFUNCTION()
	void HandleDialogueEnded(UDialogueDataAsset* EndedDialogue);

	UPROPERTY(Transient)
	TObjectPtr<UDialogueComponent> BoundDialogueComp;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayActionRuntime> CachedRuntime;
};