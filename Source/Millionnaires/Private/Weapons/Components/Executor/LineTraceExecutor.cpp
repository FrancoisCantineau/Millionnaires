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
	
	if (LaserBeamVFX && Mesh)
	{
		TraceParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(
			LaserBeamVFX,          
			Mesh,                  
			FName("Muzzle"),        
			FVector::ZeroVector,   
			FRotator::ZeroRotator,  
			EAttachLocation::SnapToTarget,
			false                   
		);

		if (TraceParticle)
		{
			TraceParticle->Deactivate();
		}
	}
}

void ULineTraceExecutor::ExecuteAttack(FWeaponContextStruct ContextStruct)
{
	Super::ExecuteAttack(ContextStruct);
	
	if (!OwnerWeapon || !OwnerWeapon->WeaponData)
		return;

	const USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	
	const FVector Start = Mesh->GetSocketLocation("Muzzle");
	const FVector Forward = Mesh->GetSocketRotation("Muzzle").Vector();
	const FVector End = Start + Forward * ContextStruct.Range;

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

	FVector TraceEndPoint = bDidHit ? Hit.ImpactPoint : End;

	if (TraceParticle)
	{
		TraceParticle->Activate();
		TraceParticle->SetNiagaraVariableVec3(ParticleVariable, TraceEndPoint);
	}

	if (bDidHit)
	{
		OnHit(Hit);
	}
	
	EndAttackExecution();
}


void ULineTraceExecutor::OnHit(const FHitResult& Hit)
{
	if (ParticleVariable != "None")
	{
		if (TraceParticle)
		{
			TraceParticle->Activate();
		
			TraceParticle->SetNiagaraVariableVec3(ParticleVariable, Hit.ImpactPoint);
		}
		
	}

	Super::OnHit(Hit);
}

void ULineTraceExecutor::EndAttackExecution()
{
	Super::EndAttackExecution();

	if (TraceParticle)
	{
		TraceParticle->Deactivate();
	}
}

