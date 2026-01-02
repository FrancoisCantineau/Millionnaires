// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Death/Behaviors/DeathRagdoll.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDeathRagdoll::Execute()
{
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FVector Impulse = Character->GetActorForwardVector()* -10000;
	Impulse.Z = 15000;
	Mesh->AddImpulseAtLocation(Impulse, Character->GetActorLocation());
	Character->GetCharacterMovement()->DisableMovement();
	
}
