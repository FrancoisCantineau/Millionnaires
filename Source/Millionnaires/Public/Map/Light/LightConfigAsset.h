// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightConfigStruct.h"
#include "Engine/DataAsset.h"
#include "LightConfigAsset.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API ULightConfigAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public :
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightConfig")
	FLightConfigStruct LightConfig;
};
