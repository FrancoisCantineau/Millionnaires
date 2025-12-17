// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectiles/ProjectileBase.h"
#include "DelayedTriggerProjectile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MILLIONNAIRES_API ADelayedTriggerProjectile : public AProjectileBase
{
	GENERATED_BODY()

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
			   const FHitResult& Hit) override;


	UPROPERTY(EditAnywhere)
	float DelayTimer;
	
};
