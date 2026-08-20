#include "QuestObjective_Counter.h"
#include "Event/GameplayEventBus.h"
#include "Core/GameplayEventContext.h"

UQuestObjective_Counter::UQuestObjective_Counter()
{
	CurrentCount = 0;
}

void UQuestObjective_Counter::Initialize(UObject* InOwner)
{
	Super::Initialize(InOwner);
	CurrentCount = 0;
}

void UQuestObjective_Counter::ObjectiveActivate()
{
	Super::ObjectiveActivate();

	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
		{
			BusSubscriptionHandle = Bus->SubscribeNative(ListenEventTag, [this](const FEventContext& Context)
			{
				HandleEvent(Context);
			});
		}
	}
}

void UQuestObjective_Counter::HandleEvent(const FEventContext& Context)
{
	if (CurrentState != EQuestObjectiveState::Active)
	{
		return;
	}

	CurrentCount = FMath::Min(CurrentCount + 1, TargetCount);

	UE_LOG(LogTemp, Warning, TEXT("QuestObjective_Counter: %d / %d (tag: %s)"), CurrentCount, TargetCount, *ListenEventTag.ToString());

	if (CurrentCount >= TargetCount)
	{
		ObjectiveComplete();
	}
	else
	{
		NotifyProgressChanged();
	}
}

void UQuestObjective_Counter::UnsubscribeFromBus()
{
	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
		{
			Bus->Unsubscribe(ListenEventTag, BusSubscriptionHandle);
		}
	}
}

void UQuestObjective_Counter::ObjectiveComplete()
{
	UnsubscribeFromBus();
	Super::ObjectiveComplete();
}

void UQuestObjective_Counter::ObjectiveFail()
{
	UnsubscribeFromBus();
	Super::ObjectiveFail();
}
