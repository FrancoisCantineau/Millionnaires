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

public:
	
	virtual void ExecuteAttack(FWeaponContextStruct ContextStruct) override;
	
protected: 
		
	

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectileBase> ProjectileClass;
    
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Spread")
	bool bUseSpread = false;
    
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Spread", meta=(EditCondition="bUseSpread"))
	float SpreadAngle = 2.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlash;
    
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* FireSound;
	
};
