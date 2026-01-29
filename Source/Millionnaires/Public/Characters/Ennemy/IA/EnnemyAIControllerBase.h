// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Characters/Ennemy/BaseEnnemyCharacter.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	UBlackboardData* BlackboardAsset;

	/** Behavior Tree */
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	ABaseEnnemyCharacter* EnemyOwner;
	
};
