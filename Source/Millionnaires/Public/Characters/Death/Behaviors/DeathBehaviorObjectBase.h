// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DeathBehaviorObjectBase.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UDeathBehaviorObjectBase : public UObject
{
	GENERATED_BODY()

protected:
	

public:
	
	UFUNCTION(BlueprintCallable)
	virtual void Execute();
	
	UPROPERTY()
	AActor* Owner;
};
