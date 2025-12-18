// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/BasicAttackComponent.h"

#include "GameFramework/Character.h"

void UBasicAttackComponent::ExecuteAbility(AActor* Target)
{
	UsedWeapon->Attack();

}

void UBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!UsedWeapon)
	{
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();

		AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			Params
		);
		UsedWeapon = Weapon;
		
		ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
		USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh();
		Weapon->AttachToComponent(
			Mesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			"HandGrip_R"
		);
		
	}
	RangeMax = UsedWeapon->GetRange();
}
