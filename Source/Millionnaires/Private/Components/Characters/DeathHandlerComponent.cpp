// Fill out your copyright notice in the Description page of Project Settings.


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

	AActor* Owner = GetOwner();

	for (auto BehaviourClass : BehaviourClasses)
	{
		UDeathBehaviorObjectBase* Behaviour =
			NewObject<UDeathBehaviorObjectBase>(this, BehaviourClass);

		Behaviour->Owner = Owner;
		Behaviours.Add(Behaviour);
	}
	
}


// Called every frame
void UDeathHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeathHandlerComponent::ExecuteDeath()
{
	if (Behaviours.Num() == 0) return;

	for (UDeathBehaviorObjectBase* Behaviour : Behaviours)
	{
		if (!Behaviour) continue;
		Behaviour->Execute();
	}
}

