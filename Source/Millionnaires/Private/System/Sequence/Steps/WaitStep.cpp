// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/Steps/WaitStep.h"

#include "System/Sequence/Data/WaitStepData.h"

void UWaitStep::Init(USequenceStepData* Data, USequenceContext* Context)
{
	Super::Init(Data, Context);
	
	UWaitStepData* WaitData = Cast<UWaitStepData>(Data);
	Duration = WaitData->Duration;
}

void UWaitStep::Start(USequenceContext* Context)
{
	Super::Start(Context);
	
	FTimerHandle Handle;

	Context->World->GetTimerManager().SetTimer(
		Handle,
		[this]()
		{
			End();
		},
		Duration,
		false
	);
}
