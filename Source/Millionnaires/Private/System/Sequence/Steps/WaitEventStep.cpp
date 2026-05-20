// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/Steps/WaitEventStep.h"

void UWaitEventStep::Start(USequenceContext* Context)
{
	Super::Start(Context);

	bFinished = false;

	if (!Context || !Context->EventBus)
		return;

	Handle = Context->EventBus->OnEvent.AddUObject(
		this,
		&UWaitEventStep::HandleEvent
	);

	bSubscribed = true;
}

void UWaitEventStep::OnFinished(USequenceContext* Context)
{
	Cleanup(Context);
}

void UWaitEventStep::BeginDestroy()
{
	Super::BeginDestroy();

	bSubscribed = false;
	Super::BeginDestroy();
}

void UWaitEventStep::HandleEvent(const FGameEvent& Event)
{
	if (bFinished)
		return;

	if (Event.EventTag == TargetEvent)
	{
		bFinished = true;
	}
}

void UWaitEventStep::Cleanup(USequenceContext* Context)
{
	if (bSubscribed && Context && Context->EventBus)
	{
		Context->EventBus->OnEvent.Remove(Handle);
		bSubscribed = false;
	}
}

