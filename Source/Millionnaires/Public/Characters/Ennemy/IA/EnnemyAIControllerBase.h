// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnnemyAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API AEnnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AEnnemyAIControllerBase();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	/** Blackboard */
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UBlackboardData* BlackboardAsset;

	/** Behavior Tree */
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UBehaviorTree* BehaviorTreeAsset;
	
};
