// Fill out your copyright notice in the Description page of Project Settings.
/* 
* Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Base class for projectiles
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Data/ProjectileData.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileBase.generated.h"

UCLASS(Blueprintable)
class MILLIONNAIRES_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:

	//* Functions */
	
	// Sets default values for this actor's properties
	AProjectileBase();
	
	void SetDamage(float InDamage) { Damage = InDamage; }

	//* Properties */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UProjectileData* ProjectileData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* TrailEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float LifeTime = 5.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
			   const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
