// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SequenceStep.h"
#include "System/Sequence/SequenceContext.h"
#include "SharedType/GameEvent.h"
#include "WaitEventStep.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UWaitEventStep : public USequenceStep
{
	GENERATED_BODY()

protected :

	UPROPERTY(EditAnywhere)
	FGameplayTag TargetEvent;

	FDelegateHandle Handle;

	bool bSubscribed = false;

public :

	virtual void Start(USequenceContext* Context) override;
	virtual void OnFinished(USequenceContext* Context);
	virtual void BeginDestroy() override;
	void HandleEvent(const FGameEvent& Event);
	void Cleanup(USequenceContext* Context);
};
