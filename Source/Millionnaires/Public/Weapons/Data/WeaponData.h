// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Datas for each weapon. Will be used by the weapon  base class and its related components
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Weapons/Enum/WeaponsEnum.h"
#include "Weapons/Struct/WeaponMovementProperties.h"

#include "WeaponData.generated.h"


class UAttackExecutorBase;
/**
 * 
 */
UCLASS(BlueprintType)
class MILLIONNAIRES_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	UTexture2D* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	bool bFullAuto;
	
	/** GAS */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	float BaseDamage = 50.f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	float AttackRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> WeaponStatsEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	//Attacks per 1 second
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	float AttackRate = 1.f; 
    
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Stats", meta = (EditCondition = "WeaponType == EWeaponType::Ranged"))
	int32 MaxAmmo = 30;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Stats", meta = (EditCondition = "WeaponType == EWeaponType::Ranged"))
	float ReloadTime = 2.f;
    
	/** SFX/VFX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation")
	USkeletalMesh* WeaponMesh;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation")
	USoundBase* AttackSound;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Presentation")
	UParticleSystem* AttackVFX;

	/** Animations */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FName AttachSocketName;


	/** Movements */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FWeaponMovementProperties MovementProperties;
	
	/** Executors */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Executor")
	UAttackExecutorBase* ExecutorType;
	
};
