// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceStep.h"
#include "SpawnStep.generated.h"

class USpawnStepData;
/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API USpawnStep : public USequenceStep
{
	GENERATED_BODY()

public :
	
	virtual void Init(USequenceStepData* Data, USequenceContext* Context) override;
	virtual void Start(USequenceContext* Context) override;

private:
	
	UPROPERTY()
	TObjectPtr<USpawnStepData> SpawnData;

	UPROPERTY()
	TObjectPtr<USequenceContext> CachedContext;
	
};
