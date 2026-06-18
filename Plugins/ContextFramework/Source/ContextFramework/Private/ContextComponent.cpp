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

void UContextComponent::AddContext(const FActiveContext& ContextData)
{
	if (!ContextData.Definition) return;
	
	if (HasContext(ContextData.Definition))
	{
		return;
	}
	for (const FContextTagEntry& Entry : ContextData.Definition->StateTags)
	{
		FContextTagChange TagChange;
		TagChange.Tag = Entry.Tag;
		TagChange.bAdded = true;

		OnContextTagChanged.Broadcast(TagChange);
	}
	
	ActiveContexts.Add(ContextData);

	OnContextAdded.Broadcast(ContextData);
	
	ApplyContextToInput(ContextData);
}

void UContextComponent::RemoveContext(const FActiveContext& ContextData)
{
	if (!HasContext(ContextData.Definition))
	{
		return;
	}

	ActiveContexts.Remove(ContextData);

	OnContextRemoved.Broadcast(ContextData);

	if (!ContextData.Definition) return;

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
			if (ContextData.Definition->InputMappingData->MappingContext)
			{
				Subsystem->RemoveMappingContext(ContextData.Definition->InputMappingData->MappingContext);
			}
		}
	}
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

