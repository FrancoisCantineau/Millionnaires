// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SequenceStepData.generated.h"

class USequenceStep;
/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API USequenceStepData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<USequenceStep> StepClass;
	
};
