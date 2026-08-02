// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayOrchestrator/Public/Actions/GameplayAction_PrintMessage.h"
#include "Core/GameplayEventContext.h"

void UGameplayAction_PrintMessage::Execute_Implementation(const FEventContext& Context)
{
	if (GEngine)
	{
		const FString Full = FString::Printf(TEXT("[GameplayOrchestrator] %s (Event: %s)"), *Message, *Context.EventTag.ToString());
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Full);
	}
	UE_LOG(LogTemp, Log, TEXT("[GameplayOrchestrator] %s"), *Message);
}
