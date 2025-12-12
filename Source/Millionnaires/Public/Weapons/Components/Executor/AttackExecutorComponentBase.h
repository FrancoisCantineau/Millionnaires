// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for attack execution, mostly regarding damages type.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponBase.h"
#include "AttackExecutorComponentBase.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAttackExecutorComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttackExecutorComponentBase();

	/**
	 * Executes the attack. Must be overrided by each child
	 * @param DamageMultiplier , multiplies the damages (used for charging weapons, for exemple)
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void ExecuteAttack(float DamageMultiplier = 1.f) PURE_VIRTUAL(UAttackExecutorComponentBase::ExecuteAttack, );

	
protected:

	//** Functions */
	
	virtual void BeginPlay() override;
	
	float GetFinalDamage(float Multiplier = 1.f) const;
	
	float GetFinalRange() const;

	//** Properties */
	
	UPROPERTY()
	AWeaponBase* OwnerWeapon;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	FName MuzzleSocketName = TEXT("Muzzle");

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
