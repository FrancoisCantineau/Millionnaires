#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_StartQuest.generated.h"

class UQuestDefinition;

/**
 * Bridge action - connects GameplayOrchestrator to QuestSystem. Lives here (in the game
 * module), never inside either plugin: it's the only piece of code in the project that needs
 * to know both UGameplayActionBase and UQuestDefinition at once.
 */
UCLASS(meta = (DisplayName = "Start Quest"))
class MILLIONNAIRES_API UGameplayAction_StartQuest : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TObjectPtr<UQuestDefinition> QuestData;

	virtual void Execute_Implementation(const FEventContext& Context) override;

#if WITH_EDITOR
	virtual FString GetEditorSummary() const override;
#endif
};