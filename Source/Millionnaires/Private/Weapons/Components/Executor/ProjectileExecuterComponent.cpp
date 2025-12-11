// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Executor/ProjectileExecuterComponent.h"

#include "Kismet/GameplayStatics.h"

void UProjectileExecuterComponent::ExecuteAttack(float DamageMultiplier)
{
	if (!OwnerWeapon || !ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileExecutor: Missing weapon or projectile class"));
		return;
	}

	// 🎨 VFX/SFX
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

	// 📍 Spawn location/rotation
	FVector SpawnLocation = OwnerWeapon->GetWeaponMesh()->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = OwnerWeapon->GetWeaponMesh()->GetSocketRotation(MuzzleSocketName);
    
	// 🎯 Spread
	if (bUseSpread)
	{
		float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
		float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
		SpawnRotation.Pitch += RandomPitch;
		SpawnRotation.Yaw += RandomYaw;
	}
    
	// 🚀 Spawn projectile
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerWeapon->GetOwner();
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
