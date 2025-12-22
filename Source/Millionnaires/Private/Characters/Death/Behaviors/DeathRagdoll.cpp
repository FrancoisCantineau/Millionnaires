// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Death/Behaviors/DeathRagdoll.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDeathRagdoll::Execute()
{
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;
	
	Character->GetCharacterMovement()->DisableMovement();
	
	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	
	Mesh->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
	
}
