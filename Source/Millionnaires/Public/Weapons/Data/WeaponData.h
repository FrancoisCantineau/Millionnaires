// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Weapons/Enum/WeaponsEnum.h"

#include "WeaponData.generated.h"

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
    
	/** Stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Stats")
	float BaseDamage = 50.f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Stats")
	float AttackRange = 200.f;

	//Attacks per 1 second
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Stats")
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
};
