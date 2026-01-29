// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "DeathHandlerComponent " - Source
 * Notes: Component to manage the death. This will execute all the different routines called whenever something dies.
 */

#include "Components/Characters/DeathHandlerComponent.h"

// Sets default values for this component's properties
UDeathHandlerComponent::UDeathHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDeathHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeComponents();
}


// Called every frame
void UDeathHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeathHandlerComponent::InitializeComponents()
{
	for (auto BehaviourClass : BehaviourClasses)
	{
		if (!BehaviourClass) continue;
        
		UDeathBehaviorObjectBase* Behaviour = 
			NewObject<UDeathBehaviorObjectBase>(
				this,              // Outer = ce component
				BehaviourClass,
				NAME_None,
				RF_NoFlags,
				nullptr,
				true,              // bCopyTransientsFromClassDefaults
				nullptr
			);
        
		if (Behaviour)
		{
			Behaviour->Owner = GetOwner();
			Behaviours.Add(Behaviour);
		}
	}
	
}

void UDeathHandlerComponent::ExecuteDeath()
{
	if (Behaviours.Num() == 0)
	{
		InitializeComponents();
	}

	for (UDeathBehaviorObjectBase* Behaviour : Behaviours)
	{
		if (!Behaviour) continue;
		Behaviour->Execute();
	}
}

