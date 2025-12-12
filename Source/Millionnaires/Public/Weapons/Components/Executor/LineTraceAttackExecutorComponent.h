// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Line Trace")
	float TraceDistance = 20000.f;

	
};
