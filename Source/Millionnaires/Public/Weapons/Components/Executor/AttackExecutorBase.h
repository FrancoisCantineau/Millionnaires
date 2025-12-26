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
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "Weapons/WeaponBase.h"
#include "AttackExecutorBase.generated.h"



UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAttackExecutorBase : public UObject
{
	GENERATED_BODY()

public:	
	
	/**
	 * Executes the attack. Must be overrided by each child
	 * @param m_DamageMultiplier , multiplies the damages (used for charging weapons, for exemple)
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void ExecuteAttack(float m_DamageMultiplier = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void EndAttackExecution();

	/**
	 * Initialize the executor, with the weapon
	 * @param Weapon, attached weapon
	 */
	virtual void Initialize(AWeaponBase* Weapon);

	
protected:

	//** Functions */

	UFUNCTION()
	virtual void OnHit(const FHitResult& Hit) ;
	
	float GetFinalDamage(float Multiplier = 1.f) const;
	
	float GetFinalRange() const;

	void ApplyDamage(const FHitResult& Hit, AActor* AttackedActor);
	
	void ExplodeAtLocation(const FHitResult& Hit);

	//** Properties */

	/** Associated weapon */
	UPROPERTY()
	AWeaponBase* OwnerWeapon;

	float DamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName MuzzleSocketName = TEXT("Muzzle");
	
	UPROPERTY(EditDefaultsOnly, Category="AOE")
	EAttackAreaType AreaType = EAttackAreaType::Single;

	UPROPERTY(EditDefaultsOnly, Category="AOE", meta=(EditCondition="AreaType != EAttackAreaType::Single"))
	float ExplosionRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="AOE", meta=(EditCondition="AreaType == EAttackAreaType::Cone"))
	float ConeAngle = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* ImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	USoundBase* ImpactSFX;
};
