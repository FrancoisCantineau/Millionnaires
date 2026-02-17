// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "DelayedTriggerProjectile" - Source
 * Notes: This class allows the projectile to trigger AFTER the first hit, as a grenade for exemple
 */

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
