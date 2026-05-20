// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Sequence/Data/SequenceStepData.h"
#include "SpawnStepData.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API USpawnStepData : public USequenceStepData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere)
	FName SpawnPointTag;

	UPROPERTY(EditAnywhere)
	FVector Offset = FVector::ZeroVector;
	
};
