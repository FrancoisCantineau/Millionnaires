// Fill out your copyright notice in the Description page of Project Settings.


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
