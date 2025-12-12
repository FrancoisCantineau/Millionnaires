// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Attack/AttackModeComponentBase.h"
#include "BeamAttackModeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UBeamAttackModeComponent : public UAttackModeComponentBase
{
	GENERATED_BODY()

public:

	virtual void StartAttacking() override;
	virtual void StopAttacking() override;

protected:

	
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float DamagePerSecond = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float AmmoPerSecond = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	float TickInterval = 0.05f; 

	UPROPERTY(EditDefaultsOnly, Category="Beam")
	UParticleSystem* BeamFX;

	FTimerHandle BeamTickTimer;

	/*void TickBeam();
	void StartBeamFX();
	void StopBeamFX();*/
};
