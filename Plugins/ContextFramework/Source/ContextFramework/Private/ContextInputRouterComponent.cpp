// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextInputRouterComponent.h"
#include "Data/ContextInputMappingDataAsset.h"
#include "InputAction.h"
#include "InputReceiverInterface.h"



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

void UContextInputRouterComponent::HandleInputReceived(const FInputActionInstance& Instance)
{
	const UInputAction* Action = Instance.GetSourceAction();

	const FGameplayTag* Tag = ActionToTagMap.Find(Action);
	if (!Tag) return;

	if (CurrentReceiver)
	{
		IInputReceiverInterface* Receiver = Cast<IInputReceiverInterface>(CurrentReceiver);
		if (Receiver)
		{
			Receiver->HandleInput(*Tag);
		}
	}
/*
	IContextInputReceiver* Receiver = GetCurrentReceiver();
	if (!Receiver) return;

	Receiver->HandleInput(
		*Tag,
		Instance.GetValue(),
		Instance.GetTriggerEvent());*/
}

