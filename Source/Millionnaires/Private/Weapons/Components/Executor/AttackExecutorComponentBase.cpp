// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, mostly regarding damages type.
 */

#include "Weapons/Components/Executor/AttackExecutorComponentBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Interfaces/DamageableInterface.h"

#include "Engine/OverlapResult.h"

#include "ProfilingDebugging/CookStats.h"


UAttackExecutorComponentBase::UAttackExecutorComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttackExecutorComponentBase::EndAttackExecution()
{
}

void UAttackExecutorComponentBase::BeginPlay()
{
	Super::BeginPlay();

	OwnerWeapon = Cast<AWeaponBase>(GetOwner());
}


void UAttackExecutorComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UAttackExecutorComponentBase::GetFinalDamage(float Multiplier) const
{
	if (!OwnerWeapon || !OwnerWeapon->BuffComponent)
		return 10.f;
    
	return OwnerWeapon->BuffComponent->GetDamage() * Multiplier;
}

float UAttackExecutorComponentBase::GetFinalRange() const
{
	if (!OwnerWeapon || !OwnerWeapon->BuffComponent)
		return 1000.f;
    
	return OwnerWeapon->BuffComponent->GetRange();
}

void UAttackExecutorComponentBase::ApplyDamage(const FHitResult& Hit, AActor* AttackedActor)
{
	OwnerWeapon->ApplyEffects(Hit, OwnerWeapon);
	
	float DamageAmount = -GetFinalDamage(DamageMultiplier);
	
	if (AttackedActor && AttackedActor->Implements<UDamageableInterface>())
	{
		IDamageableInterface::Execute_ApplyDamage(
			AttackedActor,
			DamageAmount,
			OwnerWeapon->GetOwner()
			
		);
	}

}

void UAttackExecutorComponentBase::ExplodeAtLocation(const FHitResult& Hit)
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

void UAttackExecutorComponentBase::OnHit(const FHitResult& Hit)
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
	
	switch (AreaType)
	{
	case EAttackAreaType::Single : ApplyDamage(Hit, Hit.GetActor());
		break;

	case EAttackAreaType::Sphere : ExplodeAtLocation(Hit);
		break;

	default : break;
	}
	
}
