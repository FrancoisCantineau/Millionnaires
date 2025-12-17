// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/AbilityBaseComponent.h"
#include "Weapons/WeaponBase.h"
#include "BasicAttackComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UBasicAttackComponent : public UAbilityBaseComponent
{
	GENERATED_BODY()

public :

	UPROPERTY(EditAnywhere)
	AWeaponBase* UsedWeapon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> WeaponClass;
	
	virtual bool UseAbility(AActor* Target) override;
	
};
