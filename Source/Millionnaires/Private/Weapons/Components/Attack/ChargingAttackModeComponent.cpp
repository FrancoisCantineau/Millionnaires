// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Source
 * Notes: Attack mode implementation, focused on charging attack
 */

#include "Weapons/Components/Attack/ChargingAttackModeComponent.h"

void UChargingAttackModeComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
												 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
	if (bIsCharging)
	{
		float ChargeDuration = GetWorld()->GetTimeSeconds() - ChargeStartTime;
		CurrentChargeRatio = FMath::Clamp(ChargeDuration / MaxChargeTime, 0.f, 1.f);
	}
}

void UChargingAttackModeComponent::StartAttacking()
{
	bIsAttacking = true;
	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	CurrentChargeRatio = 0.f;
}

void UChargingAttackModeComponent::StopAttacking()
{
	if (bIsCharging)
	{
		bIsCharging = false;

		DamagesMultiplier = CurrentChargeRatio;

		Attack();
		
	}
	
	
	Super::StopAttacking();
}
