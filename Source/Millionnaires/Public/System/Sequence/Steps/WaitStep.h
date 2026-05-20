// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceStep.h"
#include "WaitStep.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UWaitStep : public USequenceStep
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USequenceStep> StepClass;

	virtual void Init(USequenceStepData* Data, USequenceContext* Context) override;
	virtual void Start(USequenceContext* Context) override;

protected:

	float Duration = 0.f;
	float Elapsed = 0.f;
};
