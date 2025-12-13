// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/IA/EnnemyAIControllerBase.h"

AEnnemyAIControllerBase::AEnnemyAIControllerBase()
{
}

void AEnnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTreeAsset)
	{
		// Initialise le blackboard
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

