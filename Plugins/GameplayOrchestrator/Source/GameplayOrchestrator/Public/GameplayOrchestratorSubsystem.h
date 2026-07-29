// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayOrchestratorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayOrchestratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintCallable)
	void PublishEvent();
	
	UFUNCTION(BlueprintCallable)
	void RegisterListener();
};
