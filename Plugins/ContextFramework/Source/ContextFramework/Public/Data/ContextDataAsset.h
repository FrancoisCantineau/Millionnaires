// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ContextDataAsset.generated.h"

class UContextCameraSetupDataAsset;
/**
 * 
 */
UCLASS(Blueprintable)
class CONTEXTFRAMEWORK_API UContextDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ContextTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer BlockedActions;

	UPROPERTY(EditDefaultsOnly)
	float MoveSpeedMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bCanBeInterrupted = true;

	UPROPERTY(EditDefaultsOnly)
	UContextCameraSetupDataAsset* CameraSetupData = nullptr;
};
