// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/GameplayAction_WaitGameplayEvent.h"
#include "Actions/GameplayActionRuntime.h"
#include "Event/GameplayEventBus.h"

void UGameplayAction_WaitGameplayEvent::ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context)
{
	UWorld* World = ActionRuntime->GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UGameplayEventBus* Bus = GameInstance ? GameInstance->GetSubsystem<UGameplayEventBus>() : nullptr;

	if (!Bus || !EventTagToWaitFor.IsValid())
	{
		ActionRuntime->NotifyFinished(EGameplayActionResult::Failed);
		return;
	}

	TWeakObjectPtr<UGameplayActionRuntime> WeakActionRuntime(ActionRuntime);
	
	auto CleanupAndFinish = [WeakActionRuntime](EGameplayActionResult Result)
	{
		UGameplayActionRuntime* RT = WeakActionRuntime.Get();
		if (!RT)
		{
			return;
		}

		if (UWorld* W = RT->GetWorld())
		{
			W->GetTimerManager().ClearTimer(RT->TimerHandle);
		}
		if (UGameInstance* GI = RT->GetWorld() ? RT->GetWorld()->GetGameInstance() : nullptr)
		{
			if (UGameplayEventBus* B = GI->GetSubsystem<UGameplayEventBus>())
			{
				B->Unsubscribe(RT->SubscribedEventTag, RT->BusSubscriptionHandle);
			}
		}
		RT->NotifyFinished(Result);
	};

	const TWeakObjectPtr<AActor> ExpectedSender = Context.Sender;
	const bool bRequireSame = bRequireSameSender;
	const FGameplayTag WaitTag = EventTagToWaitFor;

	ActionRuntime->SubscribedEventTag = WaitTag;
	ActionRuntime->BusSubscriptionHandle = Bus->SubscribeNative(WaitTag, [CleanupAndFinish, ExpectedSender, bRequireSame](const FEventContext& ReceivedContext)
	{
		if (bRequireSame && ReceivedContext.Sender != ExpectedSender)
		{
			return;
		}
		CleanupAndFinish(EGameplayActionResult::Succeeded);
	});

	if (TimeoutSeconds > 0.0f && World)
	{
		FTimerDelegate TimeoutDelegate = FTimerDelegate::CreateLambda([CleanupAndFinish]()
		{
			CleanupAndFinish(EGameplayActionResult::Failed);
		});
		World->GetTimerManager().SetTimer(ActionRuntime->TimerHandle, TimeoutDelegate, TimeoutSeconds, false);
	}
}
