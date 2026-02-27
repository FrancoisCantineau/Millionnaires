// Fill out your copyright notice in the Description page of Project Settings.


/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "WeaponManagerComponent" - Source
 * Notes: Component to manage the weaopn equipement. Must probably be overrided by an inventory later 
 */

#include "Weapons/Components/WeaponsManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UWeaponsManagerComponent::UWeaponsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());

	if (OwningCharacter)
	{
		DefaultAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance()->GetClass();
	}
	
}


// Called every frame
void UWeaponsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponsManagerComponent::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (IsValid(OwningCharacter))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwningCharacter;
		SpawnParams.Instigator = OwningCharacter->GetInstigator();

		EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(
	   WeaponClass,
	   FTransform::Identity,
	   SpawnParams
   );

		if (!EquippedWeapon)
			return;
		{
			
			FName WeaponSocket = EquippedWeapon->WeaponData->AttachSocketName;
			TSubclassOf<UAnimInstance> WeaponAnimInstance = EquippedWeapon->WeaponData->AnimInstance;
			
			FAttachmentTransformRules AttachRules =
				FAttachmentTransformRules::SnapToTargetIncludingScale;

			EquippedWeapon->AttachToComponent(
				OwningCharacter->GetMesh(),
				AttachRules,
				WeaponSocket
			);

			if (WeaponAnimInstance)
			{
				OwningCharacter->GetMesh()->SetAnimInstanceClass(WeaponAnimInstance);
			}

			UCharacterMovementComponent* OwnerCharacterMovement = OwningCharacter->GetCharacterMovement();
			
			OwnerCharacterMovement->MaxWalkSpeed = EquippedWeapon->WeaponData->MovementProperties.MaxWalkSpeed;
			OwnerCharacterMovement->bOrientRotationToMovement = EquippedWeapon->WeaponData->MovementProperties.OrientRotationToMovement;
			OwnerCharacterMovement->bUseControllerDesiredRotation= EquippedWeapon->WeaponData->MovementProperties.UseControllerDesiredRotation;
		}
		
	}
}

void UWeaponsManagerComponent::UnequipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		OwningCharacter->GetMesh()->SetAnimInstanceClass(DefaultAnimInstance);

		UCharacterMovementComponent* OwnerCharacterMovement = OwningCharacter->GetCharacterMovement();
			
		OwnerCharacterMovement->MaxWalkSpeed = 500;
		OwnerCharacterMovement->bOrientRotationToMovement = true;
		OwnerCharacterMovement->bUseControllerDesiredRotation= false;
	}
}

void UWeaponsManagerComponent::EquipWeaponAlive(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!IsValid(OwningCharacter) || !WeaponClass) 
		return;
	
	AWeaponBase* WeaponToEquip = nullptr;
	for (AWeaponBase* Weapon : Inventory)
	{
		if (Weapon && Weapon->IsA(WeaponClass))
		{
			WeaponToEquip = Weapon;
			break;
		}
	}
	
	if (!WeaponToEquip)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwningCharacter;
		SpawnParams.Instigator = OwningCharacter->GetInstigator();

		WeaponToEquip = GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			FTransform::Identity,
			SpawnParams
		);

		if (!WeaponToEquip)
			return;

		Inventory.Add(WeaponToEquip);
	}
	
	if (EquippedWeapon && EquippedWeapon != WeaponToEquip)
	{
		EquippedWeapon->SetActorHiddenInGame(true);
		EquippedWeapon->SetActorEnableCollision(false);
	}


	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetActorHiddenInGame(false);
	EquippedWeapon->SetActorEnableCollision(true);


	FName WeaponSocket = EquippedWeapon->WeaponData->AttachSocketName;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	EquippedWeapon->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, WeaponSocket);

	TSubclassOf<UAnimInstance> WeaponAnimInstance = EquippedWeapon->WeaponData->AnimInstance;
	if (WeaponAnimInstance)
	{
		OwningCharacter->GetMesh()->SetAnimInstanceClass(WeaponAnimInstance);
	}
	
	UCharacterMovementComponent* OwnerCharacterMovement = OwningCharacter->GetCharacterMovement();
	OwnerCharacterMovement->MaxWalkSpeed = EquippedWeapon->WeaponData->MovementProperties.MaxWalkSpeed;
	OwnerCharacterMovement->bOrientRotationToMovement = EquippedWeapon->WeaponData->MovementProperties.OrientRotationToMovement;
	OwnerCharacterMovement->bUseControllerDesiredRotation = EquippedWeapon->WeaponData->MovementProperties.UseControllerDesiredRotation;
}

void UWeaponsManagerComponent::UnequipWeaponAlive()
{
	if (!EquippedWeapon)
		return;
	
	EquippedWeapon->SetActorHiddenInGame(true);
	EquippedWeapon->SetActorEnableCollision(false);
	
	OwningCharacter->GetMesh()->SetAnimInstanceClass(DefaultAnimInstance);
	
	UCharacterMovementComponent* OwnerCharacterMovement = OwningCharacter->GetCharacterMovement();
	OwnerCharacterMovement->MaxWalkSpeed = 500.f;
	OwnerCharacterMovement->bOrientRotationToMovement = true;
	OwnerCharacterMovement->bUseControllerDesiredRotation = false;

	EquippedWeapon = nullptr;
}

