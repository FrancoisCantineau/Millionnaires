// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sequences/GameplayActionBase.h"
#include "GameplayAction_PrintMessage.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayAction_PrintMessage : public UGameplayActionBase
{
	GENERATED_BODY()
	
	public:
    
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FString Message;
    
    
    	virtual void Execute(const FEventContext& Context) override;
	
};
