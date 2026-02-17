// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Death/Behaviors/DeathBehaviorObjectBase.h"
#include "DeathRagdoll.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UDeathRagdoll : public UDeathBehaviorObjectBase
{
	GENERATED_BODY()

public:
	
	virtual void Execute() override;
};
