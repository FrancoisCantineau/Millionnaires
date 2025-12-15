// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/BaseEnnemyCharacter.h"

void ABaseEnnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetComponents<UAbilityBaseComponent>(AbilityComponents);
	
}
