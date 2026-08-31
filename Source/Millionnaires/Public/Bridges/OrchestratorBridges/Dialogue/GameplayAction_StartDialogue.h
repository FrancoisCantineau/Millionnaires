#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_StartDialogue.generated.h"

class UDialogueDataAsset;

/**
 * Bridge action - connects GameplayOrchestrator to DialogueSystem. Lives here (in the game
 * module), never inside either plugin: it's the only piece of code in the project that needs
 * to know both UGameplayActionBase and UDialogueDataAsset at once.
 */
UCLASS(meta = (DisplayName = "Start Dialogue"))
class MILLIONNAIRES_API UGameplayAction_StartDialogue : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UDialogueDataAsset> DialogueData;

	virtual void Execute_Implementation(const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override;
#endif
};