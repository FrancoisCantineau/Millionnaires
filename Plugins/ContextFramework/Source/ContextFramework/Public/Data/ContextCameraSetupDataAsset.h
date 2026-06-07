// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ContextCameraSetupDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CONTEXTFRAMEWORK_API UContextCameraSetupDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:

	UPROPERTY(EditDefaultsOnly)
	bool bUseViewTarget = false;

	UPROPERTY(EditDefaultsOnly)
	float BlendTime = 0.3f;
	
	UPROPERTY(EditDefaultsOnly)
	float CameraYawMin = -180.f;

	UPROPERTY(EditDefaultsOnly)
	float CameraYawMax = 180.f;

	UPROPERTY(EditDefaultsOnly)
	float CameraPitchMin = -89.f;

	UPROPERTY(EditDefaultsOnly)
	float CameraPitchMax = 89.f;
};
