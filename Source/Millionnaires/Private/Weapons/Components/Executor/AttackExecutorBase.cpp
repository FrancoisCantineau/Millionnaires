// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, mostly regarding damages type.
 */

#include "Weapons/Components/Executor/AttackExecutorBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

#include "ProfilingDebugging/CookStats.h"


void UAttackExecutorBase::Initialize(AWeaponBase* Weapon)
{
	OwnerWeapon = Weapon;
}

float UAttackExecutorBase::GetFinalDamage(float Multiplier) const
{
	if (!OwnerWeapon || !OwnerWeapon->BuffComponent)
		return 10.f;
    
	return OwnerWeapon->BuffComponent->GetDamage() * Multiplier;
}

float UAttackExecutorBase::GetFinalRange() const
{
	if (!OwnerWeapon || !OwnerWeapon->BuffComponent)
		return 1000.f;
    
	return OwnerWeapon->BuffComponent->GetRange();
}

void UAttackExecutorBase::ApplyDamage(const FHitResult& Hit, AActor* AttackedActor)
{
	OwnerWeapon->ApplyEffects(Hit, OwnerWeapon);
	
	float DamageAmount = -GetFinalDamage(DamageMultiplier);
	/*
	if (AttackedActor && AttackedActor->Implements<UDamageableInterface>())
	{
		const FHitResult& Hit2 = Hit;
		IDamageableInterface::Execute_ApplyDamage(
			AttackedActor,
			DamageAmount,
			OwnerWeapon->GetOwner()
			
		);
	}*/

}

void UAttackExecutorBase::ExplodeAtLocation(const FHitResult& Hit)
{
	TArray<FOverlapResult> Overlaps;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerWeapon);
	Params.AddIgnoredActor(OwnerWeapon->GetOwner());

	
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Hit.ImpactPoint,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Actor = Result.GetActor();

		if (!Actor || Actor == OwnerWeapon->GetOwner())
			continue;

		ApplyDamage(Hit, Actor);
	}
}

void UAttackExecutorBase::OnHit(const FHitResult& Hit)
{
	FGameplayAbilityTargetDataHandle TargetData =
	UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);

	AActor* OwnerActor = OwnerWeapon->GetOwner();
	if (!OwnerActor) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!ASC) return;

	// --- ASC du hit actor
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());

	if (TargetASC && CachedGESpec.IsValid())
	{
		// Appliquer le GameplayEffectSpec
		TargetASC->ApplyGameplayEffectSpecToSelf(*CachedGESpec.Data.Get());
	}

	// --- GameplayCue
	if (ImpactCueTag.IsValid())
	{
		UAbilitySystemComponent* OwnerASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerWeapon->GetOwner());

		if (OwnerASC)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = Hit.ImpactPoint;
			CueParams.Normal = Hit.ImpactNormal;
			CueParams.PhysicalMaterial = Hit.PhysMaterial.Get();

			OwnerASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
		}
	}
	OwnerWeapon->ApplyEffects(Hit, OwnerWeapon);
	/*
	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),      
		ImpactVFX,       
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation() 
	);
	}

	if (ImpactSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this,              
		ImpactSFX,       
		Hit.ImpactPoint     
	);
	}
	
	switch (AreaType)
	{
	case EAttackAreaType::Single : ApplyDamage(Hit, Hit.GetActor());
		break;

	case EAttackAreaType::Sphere : ExplodeAtLocation(Hit);
		break;

	default : break;
	}*/
	
}

void UAttackExecutorBase::ExecuteAttack(float m_DamageMultiplier, FGameplayEffectSpecHandle GEHandle)
{
	DamageMultiplier = m_DamageMultiplier;

	CachedGESpec = GEHandle;
}

void UAttackExecutorBase::EndAttackExecution()
{
}
