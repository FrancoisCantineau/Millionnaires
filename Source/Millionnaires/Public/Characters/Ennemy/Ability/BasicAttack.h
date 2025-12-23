// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/AbilityBase.h"
#include "Weapons/WeaponBase.h"
#include "BasicAttack.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UBasicAttack : public UAbilityBase
{
	GENERATED_BODY()

public :
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> WeaponClass;
	
	virtual void ExecuteAbility(AActor* Target) override;

protected :

	UPROPERTY(EditAnywhere)
	AWeaponBase* UsedWeapon;
};
