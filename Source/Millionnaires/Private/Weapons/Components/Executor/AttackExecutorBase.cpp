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

void UAttackExecutorBase::ApplyDamage(const FHitResult& Hit, AActor* AttackedActor)
{
	
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
	ApplyGameplayEffect(Hit);
	ExecuteImpactCue(Hit);
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

void UAttackExecutorBase::ApplyGameplayEffect(const FHitResult& Hit)
{
	if (!CurrentContextStruct.DamageSpec.IsValid())
		return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());

	if (TargetASC)
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(
			*CurrentContextStruct.DamageSpec.Data.Get()
		);
	}
}

void UAttackExecutorBase::ExecuteImpactCue(const FHitResult& Hit)
{
	if (!CurrentContextStruct.ImpactCueTag.IsValid())
		return;

	UAbilitySystemComponent* InstigatorASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CurrentContextStruct.Instigator);

	if (!InstigatorASC)
		return;

	FGameplayCueParameters Params;
	Params.Location = Hit.ImpactPoint;
	Params.Normal = Hit.ImpactNormal;

	InstigatorASC->ExecuteGameplayCue(CurrentContextStruct.ImpactCueTag, Params);
}

void UAttackExecutorBase::ExecuteAttack(FWeaponContextStruct ContextStruct)
{
	CurrentContextStruct = ContextStruct;
	CurrentDamageData = ContextStruct.DamageData;
}

void UAttackExecutorBase::EndAttackExecution()
{
	if (!OwnerWeapon)
		return;

	AActor* OwnerActor = OwnerWeapon->GetOwner();
	if (!OwnerActor)
		return;

	FGameplayEventData EventData;
	EventData.Instigator = OwnerActor;
	EventData.Target = OwnerActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerActor,
		FGameplayTag::RequestGameplayTag("Event.Weapon.Attack.End"),
		EventData
	);
}
