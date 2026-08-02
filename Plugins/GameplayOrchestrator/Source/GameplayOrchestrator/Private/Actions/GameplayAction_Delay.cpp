// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayOrchestrator/Public/Actions/GameplayAction_Delay.h"
#include "Actions/GameplayActionRuntime.h"
#include "Core/GameplayEventContext.h"

void UGameplayAction_Delay::ExecuteLatent_Implementation(UGameplayActionRuntime* ActionRuntime, const FEventContext& Context)
{
	UWorld* World = ActionRuntime->GetWorld();
	if (!World)
	{
		ActionRuntime->NotifyFinished(EGameplayActionResult::Failed);
		return;
	}

	if (Duration <= 0.0f)
	{
		ActionRuntime->NotifyFinished(EGameplayActionResult::Succeeded);
		return;
	}
	
	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(ActionRuntime, &UGameplayActionRuntime::NotifyFinished, EGameplayActionResult::Succeeded);
	World->GetTimerManager().SetTimer(ActionRuntime->TimerHandle, Delegate, Duration, false);
}