// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ContextInputMappingDataAsset.generated.h"

enum class ETriggerEvent : uint8;
class UInputAction;

USTRUCT()
struct FInputTagMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<ETriggerEvent> Events;
};

/**
 * 
 */
UCLASS(Blueprintable)
class CONTEXTFRAMEWORK_API UContextInputMappingDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public :

	UPROPERTY(EditDefaultsOnly)
	TArray<FInputTagMapping> Mappings;
};
