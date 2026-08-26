// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Data/ContextStructData.h"
#include "Data/ContextDataAsset.h"
#include "Data/ContextInputMappingDataAsset.h"

// Sets default values for this component's properties
UContextComponent::UContextComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

FContextHandle UContextComponent::AddContext(FActiveContext ContextData)
{
	if (!ContextData.Definition) return FContextHandle();
	
	if (HasContext(ContextData.Definition))
	{
		return FContextHandle();
	}
	
	ContextData.Handle.GenerateNewGuid();
	
	ActiveContexts.Add(ContextData);

	OnContextAdded.Broadcast(ContextData);
	
	ApplyContextToInput(ContextData);

	return ContextData.Handle;
}

bool UContextComponent::RemoveContext(FContextHandle ContextHandle)
{
	const int32 Index = ActiveContexts.IndexOfByPredicate(
	   [&ContextHandle](const FActiveContext& Context)
	   {
		   return Context.Handle == ContextHandle;
	   });

	if (Index == INDEX_NONE) return false;

	FActiveContext ContextData = ActiveContexts[Index];
	ActiveContexts.RemoveAt(Index);

	OnContextRemoved.Broadcast(ContextData);

	if (ContextData.Definition)
	{
		for (const FContextTagEntry& Entry : ContextData.Definition->StateTags)
		{
			if (Entry.bRemoveOnEnd)
			{
				FContextTagChange TagChange;
				TagChange.Tag = Entry.Tag;
				TagChange.bAdded = false;
				OnContextTagChanged.Broadcast(TagChange);
			}
		}

		if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				if (ContextData.Definition->InputMappingData &&
					ContextData.Definition->InputMappingData->MappingContext)
				{
					Subsystem->RemoveMappingContext(ContextData.Definition->InputMappingData->MappingContext);
				}
			}
		}
	}

	const FActiveContext* NewTop = GetTopContext();
	if (NewTop)
	{
		ApplyContextToInput(*NewTop);
	}
	
	SetContextState(EContextState::Active);

	return true;
}

bool UContextComponent::HasContext(UContextDataAsset* ContextData) const
{
	for (const FActiveContext& Context : ActiveContexts)
	{
		if (Context.Definition.Get() == ContextData)
		{
			return true;
		}
	}

	return false;
}


// Called when the game starts
void UContextComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UContextComponent::ApplyContextToInput(const FActiveContext& Context)
{
	if (!Context.Definition) return;
	if (!Context.Definition->InputMappingData) return;

	UInputMappingContext* IMC = Context.Definition->InputMappingData->MappingContext;
	if (!IMC) return;

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, Context.Definition->Priority);
		}
	}
}

void UContextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

const FActiveContext* UContextComponent::GetTopContext() const
{
	if (ActiveContexts.Num() == 0)
	{
		return nullptr;
	}

	return &ActiveContexts.Last();
}

FActiveContext UContextComponent::GetTopContextBP() const
{
	return ActiveContexts.IsEmpty()
	   ? FActiveContext()
	   : ActiveContexts.Last();
}

void UContextComponent::SetContextState(EContextState ContextState)
{
	CurrentState = ContextState;

	const FActiveContext* Top = GetTopContext();
	if (!Top) return;

	if (ContextState == EContextState::Exiting && Top->Definition)
	{
		for (const FContextTagEntry& Entry : Top->Definition->StateTags)
		{
			FContextTagChange TagChange;
			TagChange.Tag = Entry.Tag;
			TagChange.bAdded = false;
			OnContextTagChanged.Broadcast(TagChange);
		}
	}
	if (ContextState == EContextState::Active && Top->Definition)
	{
		for (const FContextTagEntry& Entry : Top->Definition->StateTags)
		{
			FContextTagChange TagChange;
			TagChange.Tag = Entry.Tag;
			TagChange.bAdded = true;
			OnContextTagChanged.Broadcast(TagChange);
		}
	}

	OnContextStateChanged.Broadcast(*Top, ContextState);
}

