#include "QuestObjective.h"

UQuestObjective::UQuestObjective()
{
	CurrentState = EQuestObjectiveState::Inactive;
}

void UQuestObjective::Initialize(UObject* InOwner)
{
	Owner = TScriptInterface<IQuestObjectiveOwnerInterface>(InOwner);
	OwnerObject = InOwner;
}

UWorld* UQuestObjective::GetWorld() const
{
	return OwnerObject.IsValid() ? OwnerObject->GetWorld() : nullptr;
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

	if (bHasTimeLimit)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimeLimitHandle, this, &UQuestObjective::OnTimeLimitExpired, TimeLimitSeconds, false);
		}
	}
}

void UQuestObjective::OnTimeLimitExpired()
{
	if (CurrentState == EQuestObjectiveState::Active)
	{
		ObjectiveFail();
	}
}

float UQuestObjective::GetRemainingTime() const
{
	if (!bHasTimeLimit)
	{
		return 0.0f;
	}
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(TimeLimitHandle);
	}
	return 0.0f;
}

TArray<FQuestDisplayField> UQuestObjective::GetDisplayFields() const
{
	// Timer display is no longer here: a text snapshot would freeze at whatever value it had
	// when last captured (activation or completion), never counting down live. Instead, the
	// client computes it itself each frame from ActivationServerTime + TimeLimitSeconds -
	// both of which are cheap, one-shot values (see UQuestComponent / FQuestObjectiveRuntimeState).
	// Kept as an empty base implementation so future objective types still have a generic
	// label/value list available for anything that genuinely IS a one-shot snapshot.
	return TArray<FQuestDisplayField>();
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

	if (bHasTimeLimit)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimeLimitHandle);
		}
	}

	CurrentState = EQuestObjectiveState::Completed;

	if (Owner)
	{
		Owner->OnObjectiveCompleted(this);
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

	if (bHasTimeLimit)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimeLimitHandle);
		}
	}

	CurrentState = EQuestObjectiveState::Failed;

	if (Owner)
	{
		Owner->OnObjectiveFailed(this);
	}
}

void UQuestObjective::TickObjective(float DeltaTime)
{
	// Override in child classes for custom tick logic
}

void UQuestObjective::NotifyProgressChanged()
{
	if (Owner)
	{
		Owner->OnObjectiveProgressChanged(this);
	}
}
