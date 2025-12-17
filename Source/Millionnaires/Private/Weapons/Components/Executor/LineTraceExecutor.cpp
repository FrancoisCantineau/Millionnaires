// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, this one is used for line trace attacks.
 */


#include "Weapons/Components/Executor/LineTraceExecutor.h"
#include "Kismet/GameplayStatics.h"


void ULineTraceExecutor::Initialize(AWeaponBase* Weapon)
{
	Super::Initialize(Weapon);

	if (!OwnerWeapon) return;
	
	USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	
	if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.LaserBeamVFX && Mesh)
	{
		OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.LaserBeamVFX,          
			Mesh,                  
			FName("Muzzle"),        
			FVector::ZeroVector,   
			FRotator::ZeroRotator,  
			EAttachLocation::SnapToTarget,
			false                   
		);

		if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle)
		{
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->Deactivate();
		}
	}
}

void ULineTraceExecutor::ExecuteAttack(float m_DamageMultiplier)
{
	Super::ExecuteAttack(m_DamageMultiplier);
	
	if (!OwnerWeapon || !OwnerWeapon->WeaponData)
		return;

	const USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	
	const FVector Start = Mesh->GetSocketLocation("Muzzle");
	const FVector Forward = Mesh->GetSocketRotation("Muzzle").Vector();
	const FVector End = Start + Forward * OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerWeapon);
	Params.AddIgnoredActor(OwnerWeapon->GetOwner());

	const bool bDidHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (bDidHit)
	{
		OnHit(Hit);
	}
	else
	{
		if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle)
		{
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->Activate();
			
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->SetNiagaraVariableVec3(OwnerWeapon->WeaponData->LineTraceExecutorSettings.ParticleVariable, Hit.TraceEnd);
		}
		
	}
	
	
	float FinalDamage = GetFinalDamage(DamageMultiplier);
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Fired projectile with %.2f damage (multiplier: %.2f)"), 
			   FinalDamage, DamageMultiplier));
}


void ULineTraceExecutor::OnHit(const FHitResult& Hit)
{
	if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.ParticleVariable != "None")
	{
		if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle)
		{
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->Activate();
		
			OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->SetNiagaraVariableVec3(OwnerWeapon->WeaponData->LineTraceExecutorSettings.ParticleVariable, Hit.ImpactPoint);
		}
		
	}

	Super::OnHit(Hit);
}

void ULineTraceExecutor::EndAttackExecution()
{
	Super::EndAttackExecution();

	if (OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle)
	{
		OwnerWeapon->WeaponData->LineTraceExecutorSettings.TraceParticle->Deactivate();
	}
}

