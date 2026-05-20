// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SequenceAsset.generated.h"

class USequenceStepData;
class USequenceStep;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FStepDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<USequenceStep> StepClass;

	UPROPERTY(EditAnywhere)
	USequenceStepData* StepData;
};

UCLASS(BlueprintType)
class MILLIONNAIRES_API USequenceAsset : public UDataAsset
{
	GENERATED_BODY()

public :

	UPROPERTY(EditAnywhere)
	TArray<FStepDefinition> Steps;
};
