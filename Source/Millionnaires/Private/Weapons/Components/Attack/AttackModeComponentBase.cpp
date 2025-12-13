// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Used as the basic implementation for weapons attack.
 * This will mainly allow to choose an attack mode easily.
 */


#include "Weapons/Components/Attack/AttackModeComponentBase.h"

#include <gsl/pointers>

#include "Weapons/Components/AmmoBaseComponent.h"

// Sets default values for this component's properties
UAttackModeComponentBase::UAttackModeComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// Called when the game starts
void UAttackModeComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OwnerWeapon = Cast<AWeaponBase>(GetOwner());
	// ...
	
}


// Called every frame
void UAttackModeComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttackModeComponentBase::StartAttacking()
{
	if (!OwnerWeapon-> WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponData is null on %s"), *GetName());
		return;
	}
	
	// raise the attacking flag
	bIsAttacking = true;

	if (CanAttack())
	{
		Attack();
	}
	else
	{
		// if we're full auto, schedule the next attack
		if (OwnerWeapon->WeaponData->bFullAuto)
		{
			GetWorld()->GetTimerManager().SetTimer(ReattackTimer, this, &UAttackModeComponentBase::Attack, CooldownBetweenAttacks, false);
		}
	}
	
}

void UAttackModeComponentBase::StopAttacking()
{
	bIsAttacking = false;

	//Clears auto attack timer
	GetWorld()->GetTimerManager().ClearTimer(ReattackTimer);

	OwnerWeapon->InterruptAttack();
	
}

void UAttackModeComponentBase::Attack()
{
	
	UAmmoBaseComponent*AmmoComponent = OwnerWeapon->FindComponentByClass<UAmmoBaseComponent>();
	
	if (!AmmoComponent->Consume())
	{
		return;
	}
	OwnerWeapon->PerformAttack(DamagesMultiplier);
	
}

bool UAttackModeComponentBase::CanAttack() const
{
	float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - TimeOfLastAttack;
	return TimeSinceLastAttack >= CooldownBetweenAttacks;
}




