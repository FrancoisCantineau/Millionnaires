// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionComponent.h"

#include "Data/ContextDataAsset.h"
#include "ContextComponent.h"
#include "Data/ContextStructData.h"

// Sets default values for this component's properties
UActionComponent::UActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	UContextComponent* Contexts =
	   GetOwner()->FindComponentByClass<UContextComponent>();

	if (!Contexts)
	{
		return;
	}

	Contexts->OnContextAdded.AddUObject(
		this,
		&UActionComponent::HandleContextAdded);

	Contexts->OnContextRemoved.AddUObject(
		this,
		&UActionComponent::HandleContextRemoved);
	// ...
	
}

void UActionComponent::HandleContextAdded(const FActiveContext& ContextData)
{
	if (!ContextData.Definition) return;
	
	for (const FGameplayTag& Action :
	 ContextData.Definition->BlockedActions)
	{
		if (!Action.IsValid()) return;
		
		BlockAction(Action);
	}
}

void UActionComponent::HandleContextRemoved(const FActiveContext& ContextData)
{

	if (!ContextData.Definition) return;
	
	for (const FGameplayTag& Action :
	 ContextData.Definition->BlockedActions)
	{
		UnblockAction(Action);
	}
}


// Called every frame
void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
	
void UActionComponent::BlockAction(const FGameplayTag& ActionTag)
{
	int32& Counter = BlockCounters.FindOrAdd(ActionTag);
	Counter++;
}

void UActionComponent::UnblockAction(const FGameplayTag& ActionTag)
{
	int32* Counter = BlockCounters.Find(ActionTag);

	if (!Counter)
	{
		return;
	}

	--(*Counter);

	if (*Counter <= 0)
	{
		BlockCounters.Remove(ActionTag);
	}
}

bool UActionComponent::CanPerform(const FGameplayTag& ActionTag) const
{
	return !BlockCounters.Contains(ActionTag);
}