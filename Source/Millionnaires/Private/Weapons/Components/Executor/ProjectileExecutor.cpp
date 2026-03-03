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

void UProjectileExecutor::ExecuteAttack(FWeaponContextStruct ContextStruct)
{
	if (!OwnerWeapon || !ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileExecutor: Missing weapon or projectile class"));
		return;
	}
	
	Super::ExecuteAttack(ContextStruct);

	USkeletalMeshComponent* Mesh = OwnerWeapon->GetWeaponMesh();
	FVector MuzzleLocation = Mesh->GetSocketLocation(MuzzleSocketName);
	FVector SpawnLocation = OwnerWeapon->GetWeaponMesh()->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation =  OwnerWeapon->GetWeaponMesh()->GetSocketRotation(MuzzleSocketName);
	
	int32 Num = FMath::Max(ContextStruct.ProjectileCount, 1);
	float TotalSpread = ContextStruct.SpreadAngle;
	float HalfSpread = TotalSpread / 2.f;
	
	for (int32 i = 0; i < ContextStruct.ProjectileCount; i++)
	{
		float LerpAlpha = (Num == 1) ? 0.5f : float(i) / float(Num - 1);
		float YawOffset = FMath::Lerp(-HalfSpread, HalfSpread, LerpAlpha);

		FRotator SpawnRotation = MuzzleRotation;
		SpawnRotation.Yaw += YawOffset;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerWeapon;
		SpawnParams.Instigator = ContextStruct.Instigator;

		AProjectileBase* Projectile = OwnerWeapon->GetWorld()->SpawnActor<AProjectileBase>(
			ProjectileClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);
		if (Projectile)
		{
			Projectile->InitializeProjectile(CurrentDamageData);
		}
	}
	/*
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
			OwnerWeapon->GetWorld(), 
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
    
	AProjectileBase* Projectile = OwnerWeapon->GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass, 
		SpawnLocation, 
		SpawnRotation, 
		SpawnParams
	);
    
	if (Projectile)
	{
		Projectile->OnProjectileHit.AddDynamic(this, &UProjectileExecutor::OnHit);
	}*/
	EndAttackExecution();
}
