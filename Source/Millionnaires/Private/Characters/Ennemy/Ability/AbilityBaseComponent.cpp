// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for abilities, used by the ennemies.
 */

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

void UAbilityBaseComponent::ExecuteAbility(AActor* Target)
{
}

// Called every frame
void UAbilityBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAbilityBaseComponent::UseAbility(AActor* Target)
{
	if (!CanUseAbility(Target))
		return false;
	
	UE_LOG(LogTemp, Log, TEXT("Ability used on %s"), *Target->GetName());

	ExecuteAbility(Target);
	
	// Cooldown
	bCanUseAbility = false;
	CurrentCooldown = MaxCooldown;

	if (MaxCooldown > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			this,
			&UAbilityBaseComponent::ResetCooldown,
			MaxCooldown,
			false
		);
	}
	else
	{
		ResetCooldown();
	}

	return true;
}

bool UAbilityBaseComponent::CanUseAbility(AActor* Target)
{
	if (!Target || !OwningCharacter)
		return false;

	if (!bCanUseAbility)
		return false;
	
	const float Distance =FVector::Dist(Target->GetActorLocation(), OwningCharacter->GetActorLocation());

	if (Distance < RangeMin || Distance > RangeMax)
		return false;

	return true;
}


