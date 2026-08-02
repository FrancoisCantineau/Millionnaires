// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/GameplayActionRuntime.h"
#include "Sequence/GameplaySequenceRuntime.h"
#include "Event/GameplayEventBus.h"

UWorld* UGameplayActionRuntime::GetWorld() const
{
	if (UObject* Ctx = WorldContextObject.Get())
	{
		return Ctx->GetWorld();
	}
	return nullptr;
}

void UGameplayActionRuntime::NotifyFinished(EGameplayActionResult Result)
{
	if (bFinished)
	{
		return;
	}
	bFinished = true;

	if (UGameplaySequenceRuntime* SequenceRuntime = OwningSequenceRuntime.Get())
	{
		SequenceRuntime->OnActionRuntimeFinished(this, Result);
	}
}

void UGameplayActionRuntime::Cancel()
{
	if (bFinished)
	{
		return;
	}
	
	if (TimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
		TimerHandle.Invalidate();
	}
	
	if (SubscribedEventTag.IsValid() && BusSubscriptionHandle.IsValid())
	{
		if (UObject* Ctx = WorldContextObject.Get())
		{
			if (UGameInstance* GameInstance = Ctx->GetWorld() ? Ctx->GetWorld()->GetGameInstance() : nullptr)
			{
				if (UGameplayEventBus* Bus = GameInstance->GetSubsystem<UGameplayEventBus>())
				{
					Bus->Unsubscribe(SubscribedEventTag, BusSubscriptionHandle);
				}
			}
		}
	}
	
	if (Action)
	{
		Action->OnCancelLatent(this);
	}

	bFinished = true;
}
