// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/BasicAttack.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"

void UBasicAttack::ExecuteAbility(AActor* Target)
{
	if (!UsedWeapon)
	{
		FActorSpawnParameters Params;
		Params.Owner = OwningCharacter;

		UsedWeapon = OwningCharacter->GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			Params
		);

		ACharacter* Character = Cast<ACharacter>(OwningCharacter);
		if (Character)
		{
			UsedWeapon->AttachToComponent(
				Character->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("HandGrip_R")
			);
		}

		RangeMax = UsedWeapon->GetRange();
		MaxCooldown = 1.f / UsedWeapon->GetAttackRate();
	}

	UsedWeapon->CallAttack();

}


