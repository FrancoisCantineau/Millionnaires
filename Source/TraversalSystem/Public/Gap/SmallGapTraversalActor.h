// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraversalActor.h"
#include "SmallGapTraversalActor.generated.h"

/**
 * 
 */
UCLASS()
class TRAVERSALSYSTEM_API ASmallGapTraversalActor : public ATraversalActor
{
	GENERATED_BODY()

public :

	virtual UAnimMontage* GetMontageForContext(float Input, ETraversalHand Hand, bool bExiting, bool bEnter, bool bIsEntry, bool bIsExit) const override;
};
