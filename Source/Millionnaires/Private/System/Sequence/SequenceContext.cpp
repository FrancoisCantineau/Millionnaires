// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/SequenceContext.h"

void USequenceContext::Init(UWorld* InWorld, UGameEventSubsystem* InEventBus)
{
	World = InWorld;
	EventBus = InEventBus;
}
