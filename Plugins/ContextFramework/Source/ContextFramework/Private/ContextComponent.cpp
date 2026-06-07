// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextComponent.h"
#include "Data/ContextStructData.h"

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
	if (HasContext(ContextData.Definition))
	{
		return;
	}
	
	ActiveContexts.Add(ContextData);

	OnContextAdded.Broadcast(ContextData);
}

void UContextComponent::RemoveContext(const FActiveContext& ContextData)
{
	if (!HasContext(ContextData.Definition))
	{
		return;
	}

	ActiveContexts.Remove(ContextData);

	OnContextRemoved.Broadcast(ContextData);
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


// Called every frame
void UContextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

