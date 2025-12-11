// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Attack/AttackModeComponentBase.h"

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

	// check how much time has passed since we last attack
	const float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - TimeOfLastAttack;

	const float AttacksPerSecond = OwnerWeapon->BuffComponent->GetFireRate();
	const float CooldownBetweenAttacks = 1.f / AttacksPerSecond;
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString::Printf(TEXT("FFFFFF %f "), OwnerWeapon->BuffComponent->GetFireRate()));
	if (TimeSinceLastAttack > CooldownBetweenAttacks)
	{
		// use the weapon right away
		Attack();

	} else {

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
}

void UAttackModeComponentBase::Attack()
{
	if (!bIsAttacking)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString::Printf(TEXT("aaaaFFF %f"), OwnerWeapon->BuffComponent->GetFireRate()));
	// update the time of our last attack
	TimeOfLastAttack = GetWorld()->GetTimeSeconds();
	
	const float AttacksPerSecond = OwnerWeapon->BuffComponent->GetFireRate();
	const float CooldownBetweenAttacks = 1.f / AttacksPerSecond;

	if (OwnerWeapon->WeaponData->bFullAuto)
	{
		// schedule the next attack
		GetWorld()->GetTimerManager().SetTimer(ReattackTimer, this, &UAttackModeComponentBase::Attack, CooldownBetweenAttacks, false);
	}
}

bool UAttackModeComponentBase::CanAttack() const
{
	return false;
}




