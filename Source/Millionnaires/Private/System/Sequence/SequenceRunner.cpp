// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/SequenceRunner.h"
#include "System/Sequence/Steps/SequenceStep.h"
#include "System/Sequence/Data/SequenceAsset.h"

void USequenceRunner::Start(USequenceAsset* Asset, USequenceContext* InContext)
{
	if (!Asset || !InContext)
		return;

	Context = InContext;
	CurrentIndex = 0;
	bFinished = false;

	Steps.Empty();

	
	for (const FStepDefinition& Def : Asset->Steps)
	{
		if (!Def.StepClass)
			continue;

		USequenceStep* Step = NewObject<USequenceStep>(this, Def.StepClass);

		Step->Init(Def.StepData, Context);

		Steps.Add(Step);
	}

	if (Steps.Num() == 0)
	{
		bFinished = true;
		OnSequenceFinished.Broadcast(this);
		return;
	}

	StartStep(CurrentIndex);
}

void USequenceRunner::OnStepFinished()
{
	if (bFinished)
		return;

	CurrentIndex++;

	if (CurrentIndex >= Steps.Num())
	{
		bFinished = true;
		OnSequenceFinished.Broadcast(this);
		return;
	}

	StartStep(CurrentIndex);
}


void USequenceRunner::StartStep(int32 Index)
{
	if (!Steps.IsValidIndex(Index))
		return;

	USequenceStep* Step = Steps[Index];

	if (!Step)
	{
		OnStepFinished();
		return;
	}
	
	Step->OnFinished.AddUObject(this, &USequenceRunner::OnStepFinished);

	Step->Start(Context);
}
