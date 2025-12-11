// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Attack/AttackModeComponentBase.h"
#include "BurstAttackModeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UBurstAttackModeComponent : public UAttackModeComponentBase
{
	GENERATED_BODY()

public :
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Burst")
	int32 ShotsPerBurst = 3;

	// Delay between shots in the burst
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Burst")
	float TimeBetweenBurstShots = 0.1f;
	
	
	virtual void StopAttacking() override;

	
	virtual void Attack() override;

	void PerformBurstShot();

protected :

	int ShotsFired;
	
	FTimerHandle BurstTimerHandle;
};
