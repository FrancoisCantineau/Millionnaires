// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for attack execution, this one is used for line trace attacks.
 */

#pragma once

#include "CoreMinimal.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include "Weapons/Components/Executor/AttackExecutorBase.h"
#include "LineTraceExecutor.generated.h"



/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API ULineTraceExecutor : public UAttackExecutorBase
{
	GENERATED_BODY()

public:

	void Initialize(AWeaponBase* Weapon) override;
	
	virtual void ExecuteAttack(float DamageMultiplier = 1.f) override;

	virtual void EndAttackExecution() override;

	virtual void OnHit(const FHitResult& Hit) override;
};
