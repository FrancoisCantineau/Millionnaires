// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/ProjectileBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
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
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = LifeTime;
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
	if (OtherActor && OtherActor != GetOwner())
	{

		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);


		if (ProjectileData->ImpactParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileData->ImpactParticle, Hit.ImpactPoint, FRotator::ZeroRotator);
		if (ProjectileData->ImpactSound)
			UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->ImpactSound, Hit.ImpactPoint);

		Destroy();
	}
}
