// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Header
 * Notes: Used as the basic implementation for weapons.
 */



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Weapons/Data/WeaponData.h"
#include "Weapons/Struct/WeaponStruct.h"
#include "Weapons/Components/WeaponBuffComponent.h"

#include "WeaponBase.generated.h"


UCLASS(Blueprintable, Abstract)
class MILLIONNAIRES_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	/** Properties */

	//----Components----//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponBuffComponent* BuffComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UWeaponData* WeaponData;

	UPROPERTY(VisibleAnywhere)
	FWeaponStats RuntimeStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	/** Functions */
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartAttacking();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopAttacking();
	
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetDamage() const { return WeaponData ? WeaponData->BaseDamage : 0.f; }
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetRange() const { return WeaponData ? WeaponData->AttackRange : 0.f; }

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ApplyWeaponData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
