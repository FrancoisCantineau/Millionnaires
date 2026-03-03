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

//*GAS */
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"
#include "GameplayAbilitySystem/Attributes/WeaponAttributeSet.h"

#include "Weapons/Data/WeaponData.h"

#include "WeaponBase.generated.h"


struct FWeaponContextStruct;
struct FGameplayEffectSpecHandle;

UCLASS(Blueprintable, Abstract)
class MILLIONNAIRES_API AWeaponBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	/** Properties */

	//----Components----//
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UWeaponData* WeaponData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS", meta = (AllowPrivateAccess = "true"))
	const UWeaponAttributeSet* WeaponAttributesSet;

	/** Functions */

	//* Returns the ability system component for this actor */
	virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	void CallAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PerformAttack(FWeaponContextStruct ContextStruct);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InterruptAttack();

	virtual void OnConstruction(const FTransform& Transform) override;

	USkeletalMeshComponent* GetWeaponMesh(){return WeaponMesh;};

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeWeaponAttributes();

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UAttackExecutorBase* AttackExecutor;
	
	//* GAS */
	
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;
	
	UPROPERTY()
	bool bAttributesInitialized = false;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitialStatsGameplayEffect;
	
	//* END GAS */

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
