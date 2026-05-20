// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Subsystem/SequenceSubsystem.h"
#include "System/Sequence/SequenceContext.h"
#include "System/Sequence/SequenceRunner.h"
#include "System/Sequence/Data/SequenceAsset.h"

void USequenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EventBus = GetGameInstance()->GetSubsystem<UGameEventSubsystem>();
}

void USequenceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

USequenceContext* USequenceSubsystem::CreateContext()
{
	USequenceContext* Context = NewObject<USequenceContext>();
	Context->Init(GetWorld(), EventBus);
	return Context;
}

USequenceRunner* USequenceSubsystem::CreateRunner()
{
	return NewObject<USequenceRunner>();
}

void USequenceSubsystem::StartSequence(USequenceAsset* Asset)
{
	if (!Asset)
		return;
	
	USequenceContext* Context = NewObject<USequenceContext>();
	Context->Init(GetWorld(), EventBus);
	
	USequenceRunner* Runner = NewObject<USequenceRunner>(this);
	
	Runner->Start(Asset, Context);
	
	FActiveSequence NewSeq;
	NewSeq.Runner = Runner;
	NewSeq.Context = Context;

	ActiveSequences.Add(NewSeq);
	
	Runner->OnSequenceFinished.AddUObject(this, &USequenceSubsystem::OnSequenceFinished);
}

void USequenceSubsystem::OnSequenceFinished(USequenceRunner* Runner)
{
	for (int32 i = ActiveSequences.Num() - 1; i >= 0; i--)
	{
		if (ActiveSequences[i].Runner == Runner)
		{
			ActiveSequences.RemoveAt(i);
			break;
		}
	}
}

