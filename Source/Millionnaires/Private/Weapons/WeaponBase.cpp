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

	BuffComponent = CreateDefaultSubobject<UWeaponBuffComponent>(TEXT("BuffComponent"));

	//Add the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	
}


void AWeaponBase::SetPendingDamageMultiplier(float DamagesMultiplier)
{
	PendingDamageMultiplier = DamagesMultiplier;
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
	
	GetComponents<UWeaponEffectBaseComponent>(Effects);

	RessourceComponent = FindComponentByClass<UWeaponResourceComponentBase>();
	
	ApplyWeaponData();

	if (IsValid(AbilitySystemComponent))
	{
		WeaponAttributesSet = AbilitySystemComponent->GetSet<UWeaponAttributeSet>();
	}
	
}

void AWeaponBase::InitializeWeaponAttributes()
{
	if (!AbilitySystemComponent || !WeaponData) return;

	FGameplayEffectContextHandle Context =
		AbilitySystemComponent->MakeEffectContext();

	FGameplayEffectSpecHandle Spec =
		AbilitySystemComponent->MakeOutgoingSpec(
			InitialStatsGameplayEffect,
			1.f,
			Context
		);

	if (!Spec.IsValid()) return;

	Spec.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Weapon.Damage"),
		WeaponData->BaseDamage
	);

	Spec.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Weapon.AttackRate"),
		WeaponData->AttackRate
	);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);
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
	}
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

bool AWeaponBase::CanAttack()
{
	if (RessourceComponent)
	{
		return RessourceComponent->CanConsume();
	}
	return true;
}

void AWeaponBase::PerformAttack(FGameplayEffectSpecHandle GEHandle)
{
	if (AttackExecutor)
	{
		RessourceComponent->Consume();
		AttackExecutor->ExecuteAttack(PendingDamageMultiplier, GEHandle);
	}
}

void AWeaponBase::InterruptAttack()
{
	if (AttackExecutor)
	{
		AttackExecutor->EndAttackExecution();
	}
}

void AWeaponBase::ApplyEffects(const FHitResult& Hit, AActor* Instigatorr)
{

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	
	for (TSubclassOf<UGameplayEffect> EffectClass : WeaponData->Effects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
			Context.AddHitResult(Hit);
                
			FGameplayEffectSpecHandle SpecHandle = InstigatorASC->MakeOutgoingSpec(
				EffectClass, 1.0f, Context);
                    
			if (SpecHandle.IsValid())
			{
				InstigatorASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}
