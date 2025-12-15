// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Source
 * Notes: Implements the logic for attack effects, as fire, ice etc etc etc.
 */

#include "Weapons/Components/Effect/WeaponEffectBaseComponent.h"

// Sets default values for this component's properties
UWeaponEffectBaseComponent::UWeaponEffectBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponEffectBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UWeaponEffectBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UWeaponEffectBaseComponent::ApplyEffect(const FHitResult& Hit, AActor* Instigator)
{
}
