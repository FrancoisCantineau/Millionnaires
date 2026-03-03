// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "ProjectileBase" - Source
 * Notes: Base template for the projectile class. Overrided by every projectile
 */

#include "Weapons/Projectiles/ProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "Combat/Damages/DamageStatic.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.f);

	
	CollisionComponent->SetCollisionProfileName(TEXT("WeaponProjectile"));
	
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	RootComponent = CollisionComponent;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Trail
	/*TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(CollisionComponent);
	TrailEffect->bAutoActivate = true;*/
	
	// Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	InitialLifeSpan = LifeTime;
}

void AProjectileBase::InitializeProjectile(const FDamageData& DamageData)
{
	CurrentDamageData = DamageData;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Weapon = GetOwner())
		CollisionComponent->IgnoreActorWhenMoving(Weapon, true);

	// Ignore pawn using weapon
	if (APawn* InstigatorPawn = GetInstigator())
		CollisionComponent->IgnoreActorWhenMoving(InstigatorPawn, true);
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
							const FHitResult& Hit)
{
	StoredHit = Hit;
	ProcessHit();
}

void AProjectileBase::ProcessHit()
{
	if (!StoredHit.IsValidBlockingHit() || !StoredHit.GetActor() || StoredHit.GetActor() == GetOwner())
		return;

	OnProjectileHit.Broadcast(StoredHit);

	UDamageStatic::ApplyImpactDamageToActor(CurrentDamageData,StoredHit.GetActor(), StoredHit.ImpactPoint);
	
	if (CurrentDamageData.Radius > 0.f)
	{
		UDamageStatic::ApplyRadialDamage(CurrentDamageData, StoredHit.ImpactPoint);
	}
	
	// Spawn VFX/SFX
	if (ProjectileData->ImpactParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileData->ImpactParticle, StoredHit.ImpactPoint);
	if (ProjectileData->ImpactSound)
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->ImpactSound, StoredHit.ImpactPoint);

	// Decal
	if (ProjectileData->ImpactDecal)
	{
		FVector DecalSize(10.f, 10.f, 10.f);
		FRotator DecalRotation = StoredHit.ImpactNormal.Rotation();
		DecalRotation.Roll = FMath::FRandRange(0.f, 360.f);

		UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			ProjectileData->ImpactDecal,
			DecalSize,
			StoredHit.ImpactPoint,
			DecalRotation,
			10.f
		);
	}

	End();
}

void AProjectileBase::End()
{
	Destroy();
}
