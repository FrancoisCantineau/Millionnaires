// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/BaseCharacter.h"
#include "BaseEnnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ABaseEnnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	protected:

	ABaseEnnemyCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

public :
	
	//** Properties */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<UAbilityBase*> AbilityComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAbilityHandlerComponentBase* AbilityHandler;
};
