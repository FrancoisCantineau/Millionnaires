// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SequenceRunner.generated.h"

class USequenceAsset;
class USequenceContext;
class USequenceStep;
/**
 * 
*/



UCLASS()
class MILLIONNAIRES_API USequenceRunner : public UObject
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSequenceFinished, USequenceRunner*);
	
	
	void Start(USequenceAsset* Asset, USequenceContext* InContext);
	void OnStepFinished();

	bool IsFinished() const { return bFinished; }
	FOnSequenceFinished OnSequenceFinished;
	
private:

	void StartStep(int32 Index);
	
	UPROPERTY()
	USequenceContext* Context;

	UPROPERTY()
	TArray<USequenceStep*> Steps;

	int32 CurrentIndex = 0;
	bool bStepStarted = false;
	bool bFinished = false;
};
