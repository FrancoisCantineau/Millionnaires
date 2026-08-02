// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayOrchestrator/Public/Actions/GameplayAction_SetState.h"

void UGameplayAction_SetState::Execute_Implementation(const FEventContext& Context)
{
/*	UWorld* World = Context.Sender.IsValid() ? Context.Sender->GetWorld() : (GEngine ? GEngine->GetCurrentPlayWorld() : nullptr);
	if (!World)
	{
		return;
	}
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UWorldStateSubsystem* StateSubsystem = GameInstance->GetSubsystem<UWorldStateSubsystem>())
		{
			StateSubsystem->SetState(StateKey, StateValue, Persistence);
		}
	}*/
}

#if WITH_EDITOR
FString UGameplayAction_SetState::GetEditorSummary() const
{
	return FString::Printf(TEXT("Set State: %s = %s"), *StateKey.ToString(), *StateValue.ToString());
}
#endif