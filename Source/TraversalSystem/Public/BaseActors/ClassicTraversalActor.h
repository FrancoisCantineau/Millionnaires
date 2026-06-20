// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraversalActor.h"
#include "ClassicTraversalActor.generated.h"

/**
 * 
 */
UCLASS()
class TRAVERSALSYSTEM_API AClassicTraversalActor : public ATraversalActor
{
	GENERATED_BODY()

protected:

	virtual void MoveForward(UTraversalComponent*) override;
	virtual void MoveBackward(UTraversalComponent*) override;
	
};
