// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/IA/EnnemyAIControllerBase.h"

#include "Characters/Ennemy/BaseEnnemyCharacter.h"

AEnnemyAIControllerBase::AEnnemyAIControllerBase()
{
}

void AEnnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyOwner = Cast<ABaseEnnemyCharacter>(InPawn);
	if (!EnemyOwner || !EnemyOwner->BehaviorTree)
	{
		return;
	}

	RunBehaviorTree(EnemyOwner->BehaviorTree);
}

