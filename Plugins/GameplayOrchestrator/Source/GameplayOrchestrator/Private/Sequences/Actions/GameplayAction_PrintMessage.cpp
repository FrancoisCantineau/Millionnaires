// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequences/Actions/GameplayAction_PrintMessage.h"

void UGameplayAction_PrintMessage::Execute(const FEventContext& Context)
{
	UE_LOG(LogTemp, Warning,
		TEXT("Gameplay Event Message : %s"),
		*Message
	);
}
