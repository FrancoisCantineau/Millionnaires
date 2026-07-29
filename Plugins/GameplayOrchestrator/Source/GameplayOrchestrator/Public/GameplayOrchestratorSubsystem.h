// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayOrchestratorSubsystem.generated.h"

class UGameplaySequence;
struct FGameplayEventDefinition;
struct FEventContext;
/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayOrchestratorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :

	void PublishEvent(FGameplayTag EventTag,FEventContext Context);

	UFUNCTION(BlueprintCallable)
	void ExecuteSequence(UGameplaySequence* Sequence,
	const FEventContext& Context);
	
	UFUNCTION(BlueprintCallable)
	void RegisterListener();
	
private:

	TMap<FGameplayTag, TArray<FGameplayEventDefinition>> Triggers;
	
	

};
