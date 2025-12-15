// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Source
 * Notes: Attack mode implementation, focused on beam attack
 */

#include "Weapons/Components/Attack/BeamAttackModeComponent.h"


void UBeamAttackModeComponent::StartAttacking()
{
	if (!OwnerWeapon) return;

	bIsAttacking = true;

	GetWorld()->GetTimerManager().SetTimer(BeamTickTimer, this, &UBeamAttackModeComponent::Attack, .01, true);
	
}

void UBeamAttackModeComponent::StopAttacking()
{
	Super::StopAttacking();
	GetWorld()->GetTimerManager().ClearTimer(BeamTickTimer);
}
