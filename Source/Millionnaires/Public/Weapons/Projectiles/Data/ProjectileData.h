// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Datas for each Projectile. Will be used by the weapon mainly
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons/Projectiles/Enum/ProjectileEnum.h"
#include "ProjectileData.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UProjectileData : public UDataAsset
{
	GENERATED_BODY()

public :

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Info")
	FText ProjectileName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Info")
	EProjectileType ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
	bool bCanPenetrate = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats", meta = (EditCondition = "bCanPenetrate"))
	int32 MaxPenetrations = 1;

	//** VFX/SFX */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundBase* ImpactSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* TrailParticle;
	
};

