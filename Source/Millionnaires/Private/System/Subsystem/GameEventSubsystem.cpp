// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/GameEventSubsystem.h"

void UGameEventSubsystem::EmitEvent(const FGameEvent& Event)
{
	{
		OnEvent.Broadcast(Event);
	}
}
