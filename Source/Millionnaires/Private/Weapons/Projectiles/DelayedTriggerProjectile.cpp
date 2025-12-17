// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/DelayedTriggerProjectile.h"

void ADelayedTriggerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&AProjectileBase::ProcessHit,
		DelayTimer,
		false
	);
}
