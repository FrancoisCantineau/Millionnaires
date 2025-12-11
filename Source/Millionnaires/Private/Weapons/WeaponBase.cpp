// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Source
 * Notes: Used as the basic implementation for weapons.
 */



#include "Weapons//WeaponBase.h"



#pragma region Setup


// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Components setup */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	BuffComponent = CreateDefaultSubobject<UWeaponBuffComponent>(TEXT("BuffComponent"));
	
}

/**
* Called whenever the actor is modified
*/
void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyWeaponData();
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();


	ApplyWeaponData();
}

void AWeaponBase::ApplyWeaponData()
{
	if (WeaponData && WeaponMesh)
	{
		WeaponMesh->SetSkeletalMesh(WeaponData->WeaponMesh);
	}

	if (WeaponData)
	{
		RuntimeStats.BaseAttackSpeed = WeaponData->AttackRate;
		RuntimeStats.BaseDamage = WeaponData->BaseDamage;
		RuntimeStats.FinalAttackSpeed = RuntimeStats.BaseAttackSpeed;
		RuntimeStats.FinalDamage = RuntimeStats.BaseDamage;
	}
	if (BuffComponent)
	{
		BuffComponent->InitializeFromData(
			WeaponData->BaseDamage,
			WeaponData->AttackRate,
			WeaponData->AttackRange
		);
            
		// Debug pour vérifier
		UE_LOG(LogTemp, Warning, TEXT("✅ BuffComponent initialized - FireRate: %.2f"), 
			   BuffComponent->GetFireRate());
	}
}

#pragma endregion

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
