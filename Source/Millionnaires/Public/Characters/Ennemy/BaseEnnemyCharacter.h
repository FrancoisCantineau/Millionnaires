// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseEnnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ABaseEnnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	protected:

	virtual void BeginPlay() override;
	
};
