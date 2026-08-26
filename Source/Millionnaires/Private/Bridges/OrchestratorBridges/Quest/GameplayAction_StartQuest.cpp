#include "Bridges/OrchestratorBridges/Quest/GameplayAction_StartQuest.h"
#include "QuestComponent.h"
#include "QuestDefinition.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "Core/GameplayEventContext.h"

void UGameplayAction_StartQuest::Execute_Implementation(const FEventContext& Context)
{
	if (!QuestData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartQuest] QuestData not set, nothing started."));
		return;
	}

	APawn* Pawn = Cast<APawn>(Context.Instigator.Get());
	APlayerState* PS = Pawn ? Pawn->GetPlayerState() : Cast<APlayerState>(Context.Instigator.Get());
	UQuestComponent* QuestComp = PS ? PS->FindComponentByClass<UQuestComponent>() : nullptr;

	if (!QuestComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameplayAction_StartQuest] No UQuestComponent found on Instigator's PlayerState."));
		return;
	}

	QuestComp->StartQuest(QuestData);
}

#if WITH_EDITOR
FString UGameplayAction_StartQuest::GetEditorSummary() const
{
	return QuestData ? FString::Printf(TEXT("Start Quest: %s"), *QuestData->GetName()) : TEXT("Start Quest: (none)");
}
#endif