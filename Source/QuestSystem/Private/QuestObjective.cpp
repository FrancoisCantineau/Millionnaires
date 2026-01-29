#include "QuestObjective.h"
#include "QuestManagerComponent.h"

UQuestObjective::UQuestObjective()
{
	CurrentState = EQuestObjectiveState::Inactive;
}

void UQuestObjective::Initialize(UQuestManagerComponent* InQuestManager)
{
	QuestManager = InQuestManager;
}

/*
 * Activate the objective, setting it to active state
 */
void UQuestObjective::ObjectiveActivate()
{
	if (CurrentState != EQuestObjectiveState::Inactive)
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestObjective::ObjectiveActivate - Objective is already active or completed"));
		return;
	}

	CurrentState = EQuestObjectiveState::Active;
}

/*
 * Complete the objective, setting it to completed state
 */
void UQuestObjective::ObjectiveComplete()
{
	if (CurrentState != EQuestObjectiveState::Active)
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestObjective::ObjectiveComplete - Objective is not active"));
		return;
	}

	CurrentState = EQuestObjectiveState::Completed;

	if (QuestManager)
	{
		QuestManager->OnObjectiveCompleted(this);
	}
}

/*
 * Fail the objective, setting it to failed state
 */
void UQuestObjective::ObjectiveFail()
{
	if (CurrentState != EQuestObjectiveState::Active)
	{
		UE_LOG(LogTemp, Warning, TEXT("UQuestObjective::ObjectiveFail - Objective is not active"));
		return;
	}

	CurrentState = EQuestObjectiveState::Failed;

	if (QuestManager)
	{
		QuestManager->OnObjectiveFailed(this);
	}
}

void UQuestObjective::TickObjective(float DeltaTime)
{
	// Override in child classes for custom tick logic
}