// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, this one is used for projectile attacks.
 */

#include "Weapons/Components/Executor/ProjectileExecutor.h"

#include "Kismet/GameplayStatics.h"

void UProjectileExecutor::ExecuteAttack(float m_DamageMultiplier)
{
	Super::ExecuteAttack(m_DamageMultiplier);
	
	if (!OwnerWeapon || !OwnerWeapon->WeaponData->ProjectileExecutorSettings.ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileExecutor: Missing weapon or projectile class"));
		return;
	}

	
	if (OwnerWeapon->WeaponData->ProjectileExecutorSettings.MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(
			OwnerWeapon->WeaponData->ProjectileExecutorSettings.MuzzleFlash, 
			OwnerWeapon->GetWeaponMesh(), 
			OwnerWeapon->WeaponData->ProjectileExecutorSettings.MuzzleSocketName
		);
	}
    
	if (OwnerWeapon->WeaponData->ProjectileExecutorSettings.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			OwnerWeapon->WeaponData->ProjectileExecutorSettings.FireSound, 
			OwnerWeapon->GetActorLocation()
		);
	}


	FVector SpawnLocation = OwnerWeapon->GetWeaponMesh()->GetSocketLocation(OwnerWeapon->WeaponData->ProjectileExecutorSettings.MuzzleSocketName);
	FRotator SpawnRotation = OwnerWeapon->GetWeaponMesh()->GetSocketRotation(OwnerWeapon->WeaponData->ProjectileExecutorSettings.MuzzleSocketName);
    
	
	if (OwnerWeapon->WeaponData->ProjectileExecutorSettings.bUseSpread)
	{
		float RandomPitch = FMath::RandRange(-OwnerWeapon->WeaponData->ProjectileExecutorSettings.SpreadAngle, OwnerWeapon->WeaponData->ProjectileExecutorSettings.SpreadAngle);
		float RandomYaw = FMath::RandRange(-OwnerWeapon->WeaponData->ProjectileExecutorSettings.SpreadAngle, OwnerWeapon->WeaponData->ProjectileExecutorSettings.SpreadAngle);
		SpawnRotation.Pitch += RandomPitch;
		SpawnRotation.Yaw += RandomYaw;
	}
    

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerWeapon;
	SpawnParams.Instigator = Cast<APawn>(OwnerWeapon->GetOwner());
    
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		OwnerWeapon->WeaponData->ProjectileExecutorSettings.ProjectileClass, 
		SpawnLocation, 
		SpawnRotation, 
		SpawnParams
	);
    
	if (Projectile)
	{
		Projectile->OnProjectileHit.AddDynamic(this, &UProjectileExecutor::OnHit);
	}
}
