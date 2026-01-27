// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AbilityChoice.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MILLIONNAIRES_API UBTTask_AbilityChoice : public UBTTaskNode
{
	GENERATED_BODY()

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelectedAbilityIndexKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FGameplayTag SelectedAbilityTag;
};
