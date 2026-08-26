#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_StopQuest.generated.h"

class UQuestDefinition;

UENUM(BlueprintType)
enum class EQuestOutcome : uint8
{
	Failed,
	Completed
};

UCLASS(meta = (DisplayName = "Stop Quest"))
class MILLIONNAIRES_API UGameplayAction_StopQuest : public UGameplayActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TObjectPtr<UQuestDefinition> QuestData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestOutcome Outcome = EQuestOutcome::Completed;

	virtual void Execute_Implementation(const FEventContext& Context) override;
		
};