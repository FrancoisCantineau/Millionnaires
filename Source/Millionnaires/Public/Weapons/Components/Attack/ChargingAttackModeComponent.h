// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Header
 * Notes: Attack mode implementation, focused on charging attack
 */

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Attack/AttackModeComponentBase.h"
#include "ChargingAttackModeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UChargingAttackModeComponent : public UAttackModeComponentBase
{
	GENERATED_BODY()

public :

	//** Functions */
	
	virtual void StartAttacking() override;
	virtual void StopAttacking() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//** Properties */

	UPROPERTY(EditAnywhere)
	float MaxChargeTime = 2.f;

protected:
	
	float CurrentChargeRatio = 0.f;
	
	bool bIsCharging = false;
	
	float ChargeStartTime = 0.f;
};
