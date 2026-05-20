// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "System/Sequence/Data/SequenceAsset.h"
#include "SequenceSubsystem.generated.h"

/**
 * 
 */
 
 class USequenceRunner;
 class USequenceContext;
 class USequenceStep;
 class UGameEventSubsystem;

USTRUCT()
struct FActiveSequence
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class USequenceRunner> Runner;

	UPROPERTY()
	TObjectPtr<class USequenceContext> Context;
};


UCLASS()
class MILLIONNAIRES_API USequenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void StartSequence(USequenceAsset* Asset);

	
private:
	
	void OnSequenceFinished(USequenceRunner* Runner);

	UPROPERTY()
	TArray<FActiveSequence> ActiveSequences;

	UPROPERTY()
	TObjectPtr<UGameEventSubsystem> EventBus;

	USequenceContext* CreateContext();
	USequenceRunner* CreateRunner();
};
