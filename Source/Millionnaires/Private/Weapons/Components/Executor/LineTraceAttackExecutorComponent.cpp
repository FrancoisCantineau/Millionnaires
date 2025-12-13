// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, this one is used for line trace attacks.
 */


#include "Weapons/Components/Executor/LineTraceAttackExecutorComponent.h"
#include "Kismet/GameplayStatics.h"


void ULineTraceAttackExecutorComponent::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	
	if (TraceParticle)
	{
		TraceParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(
	   LaserBeamVFX,           // Niagara System
	   Mesh,                   // Parent
	   FName("Muzzle"),        // Socket Name
	   FVector::ZeroVector,    // Location Offset
	   FRotator::ZeroRotator,  // Rotation Offset
	   EAttachLocation::SnapToTarget, // Snap to socket
	   false               // bAutoDestroy
   );
		TraceParticle->Deactivate();
	}
	
	
}

void ULineTraceAttackExecutorComponent::ExecuteAttack(float m_DamageMultiplier)
{
	DamageMultiplier = m_DamageMultiplier;
	
	if (!OwnerWeapon || !OwnerWeapon->WeaponData)
		return;

	const USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	
	const FVector Start = Mesh->GetSocketLocation("Muzzle");
	const FVector Forward = Mesh->GetSocketRotation("Muzzle").Vector();
	const FVector End = Start + Forward * TraceDistance;

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
		if (TraceParticle)
		{
			TraceParticle->Activate();
			
			TraceParticle->SetNiagaraVariableVec3(ParticleVariable, Hit.TraceEnd);
		}
		
	}
	
	
	float FinalDamage = GetFinalDamage(DamageMultiplier);
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Fired projectile with %.2f damage (multiplier: %.2f)"), 
			   FinalDamage, DamageMultiplier));
}


void ULineTraceAttackExecutorComponent::OnHit(const FHitResult& Hit)
{
	OwnerWeapon->ApplyEffects(Hit, OwnerWeapon);

	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),      
		ImpactVFX,       
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation() 
	);
	}
	
	if (ParticleVariable != "None")
	{
		if (TraceParticle)
		{
			TraceParticle->Activate();
		
			TraceParticle->SetNiagaraVariableVec3(ParticleVariable, Hit.ImpactPoint);
		}
		
	}

	ApplyDamage(Hit.GetActor());
}

void ULineTraceAttackExecutorComponent::EndAttackExecution()
{
	Super::EndAttackExecution();

	if (TraceParticle)
	{
		TraceParticle->Deactivate();
	}
}

