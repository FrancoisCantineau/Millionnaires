#include "Bridges/OrchestratorBridges/Quest/GameplayAction_StopQuest.h"
#include "QuestComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "QuestDefinition.h"
#include "Core/GameplayEventContext.h"

void UGameplayAction_StopQuest::Execute_Implementation(const FEventContext& Context)
{
	if (!IsValid(QuestData.Get())) return;
	APawn* Pawn = Cast<APawn>(Context.Instigator.Get());
	APlayerState* PS = Pawn ? Pawn->GetPlayerState() : Cast<APlayerState>(Context.Instigator.Get());
	UQuestComponent* QuestComp = PS ? PS->FindComponentByClass<UQuestComponent>() : nullptr;
	if (!QuestComp) return;

	if (Outcome == EQuestOutcome::Completed)
		QuestComp->CompleteQuestById(QuestData->QuestId);
	else
		QuestComp->FailQuestById(QuestData->QuestId);
}