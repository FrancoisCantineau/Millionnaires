// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, mostly regarding damages type.
 */

#include "Weapons/Components/Executor/AttackExecutorBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Interfaces/DamageableInterface.h"

#include "Engine/OverlapResult.h"
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
	
	if (AttackedActor && AttackedActor->Implements<UDamageableInterface>())
	{
		const FHitResult& Hit2 = Hit;
		IDamageableInterface::Execute_ApplyDamage(
			AttackedActor,
			DamageAmount,
			OwnerWeapon->GetOwner()
			
		);
	}

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
	}
	
}

void UAttackExecutorBase::ExecuteAttack(float m_DamageMultiplier)
{
	DamageMultiplier = m_DamageMultiplier;
}

void UAttackExecutorBase::EndAttackExecution()
{
}
