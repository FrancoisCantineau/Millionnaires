// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/AbilityBaseComponent.h"
#include "Characters/BaseCharacter.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAbilityBaseComponent::UAbilityBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbilityBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ABaseCharacter>(GetOwner());
	
}

void UAbilityBaseComponent::ResetCooldown()
{
	bCanUseAbility = true;
}

// Called every frame
void UAbilityBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAbilityBaseComponent::UseAbility()
{
	return false;
}

bool UAbilityBaseComponent::CanUseAbility()
{
	return bCanUseAbility;
}


