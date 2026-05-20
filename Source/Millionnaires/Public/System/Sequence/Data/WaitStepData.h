// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Sequence/Data/SequenceStepData.h"
#include "WaitStepData.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UWaitStepData : public USequenceStepData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float Duration = 1.f;
};
