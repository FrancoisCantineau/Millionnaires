// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/WeaponAnimationHandlerComponent.h"

// Sets default values for this component's properties
UWeaponAnimationHandlerComponent::UWeaponAnimationHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponAnimationHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponAnimationHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponAnimationHandlerComponent::PlayMontage(UAnimMontage* Montage, USkeletalMeshComponent* TargetMesh,
	float PlayRate)
{
	float animRate = PlayRate*Montage->GetPlayLength();
	if (!Montage || !TargetMesh) return;

	UAnimInstance* AnimInst = TargetMesh->GetAnimInstance();
	if (AnimInst)
	{
	AnimInst->Montage_Play(Montage, animRate);
	}
}

