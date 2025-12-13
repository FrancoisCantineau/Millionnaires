// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack execution, this one is used for projectile attacks.
 */

#include "Weapons/Components/Executor/ProjectileExecuterComponent.h"

#include "Kismet/GameplayStatics.h"

void UProjectileExecuterComponent::ExecuteAttack(float m_DamageMultiplier)
{
	DamageMultiplier = m_DamageMultiplier;
	
	if (!OwnerWeapon || !ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileExecutor: Missing weapon or projectile class"));
		return;
	}

	
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(
			MuzzleFlash, 
			OwnerWeapon->GetWeaponMesh(), 
			MuzzleSocketName
		);
	}
    
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, 
			FireSound, 
			OwnerWeapon->GetActorLocation()
		);
	}


	FVector SpawnLocation = OwnerWeapon->GetWeaponMesh()->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = OwnerWeapon->GetWeaponMesh()->GetSocketRotation(MuzzleSocketName);
    
	
	if (bUseSpread)
	{
		float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
		float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
		SpawnRotation.Pitch += RandomPitch;
		SpawnRotation.Yaw += RandomYaw;
	}
    

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerWeapon;
	SpawnParams.Instigator = Cast<APawn>(OwnerWeapon->GetOwner());
    
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass, 
		SpawnLocation, 
		SpawnRotation, 
		SpawnParams
	);
    
	if (Projectile)
	{
		float FinalDamage = GetFinalDamage(DamageMultiplier);
		
		Projectile->SetDamage(FinalDamage);

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Fired projectile with %.2f damage (multiplier: %.2f)"), 
			   FinalDamage, DamageMultiplier));
	}
}
