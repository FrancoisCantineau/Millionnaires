// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Used as the basic implementation for weapons attack.
 * This will mainly allow to choose an attack mode easily.
 */


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponBase.h"

#include "AttackModeComponentBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackExecuted, float, DamageMultiplier);

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAttackModeComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:

	//** Functions */
	
	// Sets default values for this component's properties
	UAttackModeComponentBase();

	UFUNCTION(BlueprintCallable, Category = "Attack Mode")
	virtual void StartAttacking();
	
	UFUNCTION(BlueprintCallable, Category = "Attack Mode")
	virtual void StopAttacking();

	UFUNCTION(Blueprintable, Category = "Attack Mode")
	virtual void Attack();

	UFUNCTION(BlueprintPure, Category = "Attack Mode")
	virtual bool CanAttack() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//** Properties */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeaponBase* OwnerWeapon = nullptr;

	//If true, the weapon is currently attacking
	bool bIsAttacking = false;

	//Game time of last attack
	float TimeOfLastAttack = 0.0f;
	
	// Timer to handle full auto attack
	FTimerHandle ReattackTimer;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
