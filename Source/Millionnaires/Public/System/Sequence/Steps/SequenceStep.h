// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Sequence/SequenceContext.h"
#include "UObject/Object.h"
#include "SequenceStep.generated.h"

class USequenceStepData;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnStepFinished);

UCLASS(Blueprintable,Abstract)
class MILLIONNAIRES_API USequenceStep : public UObject
{
	GENERATED_BODY()

public:
	
	virtual void Init(USequenceStepData* Data, USequenceContext* Context);
	virtual void Start(USequenceContext* Context) {}
	virtual void Tick(USequenceContext* Context, float DeltaTime) {}
	virtual void End();
	virtual bool IsFinished() const { return bFinished; }
	
	bool bStarted = false;
	
	FOnStepFinished OnFinished;
protected:
	
	bool bFinished = false;
	
};
