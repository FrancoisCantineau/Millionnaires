// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/Steps/SequenceStep.h"

void USequenceStep::Init(USequenceStepData* Data, USequenceContext* Context)
{
	
}

void USequenceStep::End()
{
	OnFinished.Broadcast();
}
