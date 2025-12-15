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

#include "Weapons/Components/Executor/AttackExecutorComponentBase.h"
#include "LineTraceAttackExecutorComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API ULineTraceAttackExecutorComponent : public UAttackExecutorComponentBase
{
	GENERATED_BODY()

public:
	
	virtual void ExecuteAttack(float DamageMultiplier = 1.f) override;

	virtual void EndAttackExecution() override;

	virtual void BeginPlay() override;

	void OnHit(const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	float TraceDistance = 20000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	UNiagaraComponent* TraceParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* LaserBeamVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	FString ParticleVariable = "None";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* ImpactVFX;
	
	
};
