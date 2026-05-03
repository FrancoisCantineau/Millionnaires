// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Source
 * Notes: Used as the basic implementation for weapons.
 */



#include "Weapons//WeaponBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Weapons/Components/Executor/AttackExecutorBase.h"



#pragma region Setup

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Components setup */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	//Add the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	
}

/**
* Called whenever the actor is modified
*/
void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AttackExecutor = WeaponData->ExecutorType;

	if (AttackExecutor)
	{
		AttackExecutor->Initialize(this);
	}

	if (!bAttributesInitialized)
	{
		InitializeWeaponAttributes();
		bAttributesInitialized = true;
	}
	
	if (IsValid(AbilitySystemComponent))
	{
		WeaponAttributesSet = AbilitySystemComponent->GetSet<UWeaponAttributeSet>();
	}
	
}

void AWeaponBase::InitializeWeaponAttributes()
{
	if (!AbilitySystemComponent || !WeaponData || !InitialStatsGameplayEffect) return;

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		InitialStatsGameplayEffect,
		1.f,
		Context
	);

	if (!SpecHandle.IsValid()) return;
	
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.MaxAmmo"), WeaponData->MaxAmmo);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.CurrentAmmo"), WeaponData->BaseAmmo);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.Damage"), WeaponData->BaseDamage);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.AttackRate"), WeaponData->AttackRate);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.AttackRange"), WeaponData->AttackRange);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Weapon.ProjectilesCount"), WeaponData->ProjectilesPerShot);

	// Appliquer à l'ASC
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}


#pragma endregion

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AWeaponBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AWeaponBase::PerformAttack(FWeaponContextStruct ContextStruct)
{
	if (AttackExecutor)
	{
		AttackExecutor->ExecuteAttack(ContextStruct);
	}
}

void AWeaponBase::InterruptAttack()
{
	if (AttackExecutor)
	{
		AttackExecutor->EndAttackExecution();
	}
}
