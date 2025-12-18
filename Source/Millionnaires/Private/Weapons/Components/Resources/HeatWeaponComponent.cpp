// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Resources/HeatWeaponComponent.h"

bool UHeatWeaponComponent::CanConsume() const
{
	return CurrentHeat < OverheatThreshold;
}

bool UHeatWeaponComponent::Consume()
{
	if (!CanConsume()) return false;

	CurrentHeat += HeatPerShot;
	
	ChangesApplied(CurrentHeat, OverheatThreshold);

	LastUse = GetWorld()->GetTimeSeconds();
	
	return true;
}

float UHeatWeaponComponent::GetResourcePercent() const
{
	return Super::GetResourcePercent();
}

void UHeatWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ChangesApplied(CurrentHeat, OverheatThreshold);
}

void UHeatWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CurrentHeat > 0.f && GetWorld()->GetTimeSeconds() - LastUse > MinimumTimeBeforeCooling)
	{
		CurrentHeat = FMath::Max(0.f, CurrentHeat - CoolingRate * DeltaTime);
		ChangesApplied(CurrentHeat, OverheatThreshold);
	}
}
