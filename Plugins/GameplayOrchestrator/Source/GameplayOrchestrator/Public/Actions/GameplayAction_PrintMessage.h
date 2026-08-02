// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayActionBase.h"
#include "GameplayAction_PrintMessage.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayAction_PrintMessage : public UGameplayActionBase
{
	GENERATED_BODY()
	
	public:
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Print Debug Message")
	FString Message = TEXT("Event triggered");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Print Debug Message")
	FColor Color = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Print Debug Message")
	float Duration = 5.0f;

	virtual void Execute_Implementation(const FEventContext& Context) override;
};
