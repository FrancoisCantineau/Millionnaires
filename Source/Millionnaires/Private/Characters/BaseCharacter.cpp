/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "Francki"
 * Class: "BaseCharacter" - Source
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("BPC_StatsComponent"));
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (StatsComponent)
    {
        StatsComponent->OnDeath.AddDynamic(this, &ABaseCharacter::Die);
    }
}

void ABaseCharacter::ApplyDamage_Implementation(float Damage, AActor* DamageCauser)
{
    StatsComponent->ModifyHealth(Damage);
}

void ABaseCharacter::Die()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
    }
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (MeshComp)
    {
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
        MeshComp->SetSimulatePhysics(true);
        MeshComp->WakeAllRigidBodies();
    }
}


