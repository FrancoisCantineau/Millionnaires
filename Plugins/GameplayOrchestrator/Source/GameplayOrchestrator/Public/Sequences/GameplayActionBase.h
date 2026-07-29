// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayActionBase.generated.h"

struct FEventContext;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class GAMEPLAYORCHESTRATOR_API UGameplayActionBase : public UObject
{
	GENERATED_BODY()

public:

	virtual void Execute(const FEventContext& Context);
};
