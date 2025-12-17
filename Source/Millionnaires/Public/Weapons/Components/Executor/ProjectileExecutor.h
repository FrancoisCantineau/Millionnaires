// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for attack execution, this one is used for projectile attacks.
 */


#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Executor/AttackExecutorBase.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "ProjectileExecutor.generated.h"



/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UProjectileExecutor : public UAttackExecutorBase
{
	GENERATED_BODY()

protected:
	

public :
		
	virtual void ExecuteAttack(float DamageMultiplier = 1.f) override;
};
