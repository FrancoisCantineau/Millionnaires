// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Subsystem/GameEventSubsystem.h"
#include "UObject/Object.h"
#include "SequenceContext.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API USequenceContext : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UGameEventSubsystem* EventBus = nullptr;

	UWorld* World = nullptr;

	void Init(UWorld* InWorld, UGameEventSubsystem* InEventBus);
	
};
