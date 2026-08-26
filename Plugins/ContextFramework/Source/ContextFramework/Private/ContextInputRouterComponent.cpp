// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextInputRouterComponent.h"

#include "ContextComponent.h"
#include "Data/ContextInputMappingDataAsset.h"
#include "InputAction.h"
#include "Data/ContextStructData.h"
#include "Data/ContextDataAsset.h"
#include "Interfaces/InputReceiverInterface.h"



// Sets default values for this component's properties
UContextInputRouterComponent::UContextInputRouterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UContextInputRouterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!InputMappingDataAsset)
	{
		return;
	}

	ActionToTagMap.Empty();

	for (const FInputTagMapping& Mapping : InputMappingDataAsset->Mappings)
	{
		ActionToTagMap.Add(Mapping.Action, Mapping.Tag);
	}
	
}


// Called every frame
void UContextInputRouterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UContextInputRouterComponent::HandleInputReceived(
	const FInputActionInstance& Instance)
{
	if (!CachedContextComponent)
	{
		return;
	}

	const UInputAction* Action = Instance.GetSourceAction();

	const FGameplayTag* Tag = ActionToTagMap.Find(Action);

	if ( CachedContextComponent->GetContextState() != EContextState::Active)
		return;
	
	if (!Tag || !CurrentReceiver)
		return;

	if (CurrentReceiver->Implements<UInputReceiverInterface>())
	{
		IInputReceiverInterface::Execute_HandleInput(
			CurrentReceiver,
			*Tag,
			Instance.GetValue(),
			Instance.GetTriggerEvent()
		);
	}
}


void UContextInputRouterComponent::Initialize(UContextComponent* InContext)
{
	CachedContextComponent = InContext;
	
	if (CachedContextComponent)
	{
		CachedContextComponent->OnContextAdded.AddUObject(
			this,
			&ThisClass::OnContextAdded);

		CachedContextComponent->OnContextRemoved.AddUObject(
			this,
			&ThisClass::OnContextRemoved);
	}
}

void UContextInputRouterComponent::OnContextAdded(const FActiveContext& Context)
{
	if (Context.Definition)
	{
		const UContextInputMappingDataAsset* NewMapping =
		Context.Definition->InputMappingData.Get();

		ActiveMapping = NewMapping;

		BuildActionMap();
	}

	
	CurrentReceiver = Context.InputReceiver;

	
}
void UContextInputRouterComponent::OnContextRemoved(const FActiveContext& Context)
{
	if (Context.InputReceiver == CurrentReceiver)
	{
		ActiveMapping = nullptr;
		CurrentReceiver = nullptr;
	}

	const FActiveContext* NewTopContext =
		CachedContextComponent
			? CachedContextComponent->GetTopContext()
			: nullptr;

	if (NewTopContext && NewTopContext->Definition)
	{
		const UContextInputMappingDataAsset* Mapping =
			NewTopContext->Definition->InputMappingData.Get();

		ActiveMapping = Mapping;
		CurrentReceiver = NewTopContext->InputReceiver;
	}

	BuildActionMap();
}
void UContextInputRouterComponent::BuildActionMap()
{
	ActionToTagMap.Reset();

	if (!ActiveMapping)
	{
		return;
	}

	for (const FInputTagMapping& Mapping : ActiveMapping->Mappings)
	{
		if (Mapping.Action)
		{
			ActionToTagMap.Add(
				Mapping.Action.Get(),
				Mapping.Tag);
		}
	}
}