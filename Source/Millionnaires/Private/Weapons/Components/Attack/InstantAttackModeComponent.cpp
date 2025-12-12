// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Source
 * Notes: Attack mode implementation, focused on instant attack
 */


#include "Weapons/Components/Attack/InstantAttackModeComponent.h"

#include "Weapons/Components/AmmoBaseComponent.h"

void UInstantAttackModeComponent::Attack()
{
	if (!bIsAttacking || !CanAttack())
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString::Printf(TEXT("aaaaFFF %f"), OwnerWeapon->BuffComponent->GetFireRate()));
	// update the time of our last attack
	TimeOfLastAttack = GetWorld()->GetTimeSeconds();
	const float AttacksPerSecond = OwnerWeapon->BuffComponent->GetFireRate();
	CooldownBetweenAttacks = 1.f / AttacksPerSecond;
	
	Super::Attack();
	
	if (OwnerWeapon->WeaponData->bFullAuto)
	{
		// schedule the next attack
		GetWorld()->GetTimerManager().SetTimer(ReattackTimer, this, &UAttackModeComponentBase::Attack, CooldownBetweenAttacks, false);
	}
}
