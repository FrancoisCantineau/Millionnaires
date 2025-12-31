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
#include "Components/WeaponAnimationHandlerComponent.h"
#include "Components/Effect/WeaponEffectBaseComponent.h"
#include "Components/Resources/WeaponResourceComponentBase.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponAnimationHandlerComponent* AnimationComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UWeaponResourceComponentBase*RessourceComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UWeaponData* WeaponData;

	UPROPERTY(VisibleAnywhere)
	FWeaponStats RuntimeStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	/** Functions */
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void CallAttack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartAttacking();

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, Category = "Weapon")
	void StopAttacking();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InterruptAttack();
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetDamage() const { return WeaponData ? WeaponData->BaseDamage : 0.f; }
    
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetRange() const { return WeaponData ? WeaponData->AttackRange : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetAttackRate() const { return WeaponData ? WeaponData->AttackRate : 0.f; }

	UFUNCTION(BlueprintCallable)
	void SetPendingDamageMultiplier(float DamagesMultiplier);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ApplyWeaponData();

	virtual void ApplyEffects(const FHitResult& Hit, AActor* Instigatorr);

	USkeletalMeshComponent* GetWeaponMesh(){return WeaponMesh;};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HitScanEnable(bool bShouldEnable);

	void HitScanEnable_Implementation(bool bShouldEnable){};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//** Properties */

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TArray<UWeaponEffectBaseComponent*> Effects;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UAttackExecutorBase* AttackExecutor;
	
	float PendingDamageMultiplier = 1.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
