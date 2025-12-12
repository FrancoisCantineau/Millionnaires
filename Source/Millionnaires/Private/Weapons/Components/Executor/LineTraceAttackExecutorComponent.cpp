// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Executor/LineTraceAttackExecutorComponent.h"

void ULineTraceAttackExecutorComponent::ExecuteAttack(float DamageMultiplier)
{
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
		// Debug
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, 1.f, 0, 1.f);

		OwnerWeapon->ApplyEffects(Hit, OwnerWeapon);
	}

	
	
	float FinalDamage = GetFinalDamage(DamageMultiplier);
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Fired projectile with %.2f damage (multiplier: %.2f)"), 
			   FinalDamage, DamageMultiplier));
}
