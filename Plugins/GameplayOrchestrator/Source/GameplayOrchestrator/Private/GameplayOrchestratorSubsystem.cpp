// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayOrchestratorSubsystem.h"
#include "Core/GameplayEventContext.h"
#include "Sequences/GameplayActionBase.h"

void UGameplayOrchestratorSubsystem::PublishEvent(FGameplayTag EventTag, FEventContext Context)
{
}

void UGameplayOrchestratorSubsystem::RegisterListener()
{
}

void UGameplayOrchestratorSubsystem::ExecuteSequence(UGameplaySequence* Sequence, const FEventContext& Context)
{
	for(const FGameplaySequenceStep& Step : Sequence->Steps)
	{
		for(UGameplayActionBase* Action : Step.ParallelActions)
		{
			if(Action)
			{
				Action->Execute(Context);
			}
		}
	}
}
