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
#include "Weapons/Components/Executor/AttackExecutorComponentBase.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "ProjectileExecuterComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UProjectileExecuterComponent : public UAttackExecutorComponentBase
{
	GENERATED_BODY()

protected:

	//** Properties */

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectileBase> ProjectileClass;
    
	UPROPERTY(EditAnywhere, Category = "Projectile|Spread")
	bool bUseSpread = false;
    
	UPROPERTY(EditAnywhere, Category = "Projectile|Spread", meta=(EditCondition="bUseSpread"))
	float SpreadAngle = 2.f;
    
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFlash;
    
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* FireSound;

public :
		
	virtual void ExecuteAttack(float DamageMultiplier = 1.f) override;
};
