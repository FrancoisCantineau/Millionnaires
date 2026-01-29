// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/BasicAttack.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"

void UBasicAttack::ExecuteAbility_Implementation(AActor* Owner, AActor* Target, const UAbilityDataAsset* Data)
{
	Super::ExecuteAbility_Implementation(Owner, Target, Data);
	
	if (!UsedWeapon)
	{
		FActorSpawnParameters Params;
		Params.Owner = Owner;
        
		UsedWeapon = Owner->GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			Params
		);
        
		ACharacter* Character = Cast<ACharacter>(Owner);
		if (Character && UsedWeapon)
		{
			UsedWeapon->AttachToComponent(
				Character->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("HandGrip_R")
			);
		}
	}
	
	if (UsedWeapon)
	{
		UsedWeapon->CallAttack();
	}
}
