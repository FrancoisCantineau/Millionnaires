// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/BaseEnnemyCharacter.h"

ABaseEnnemyCharacter::ABaseEnnemyCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	AbilityHandler = CreateDefaultSubobject<UAbilityHandlerComponentBase>(TEXT("BPC_AbilityHandlerComponent"));
}

void ABaseEnnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
