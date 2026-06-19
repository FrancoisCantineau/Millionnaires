// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ContextDataAsset.generated.h"

class UContextInputMappingDataAsset;
class UContextCameraSetupDataAsset;
/**
 * 
 */


USTRUCT(Blueprintable)
struct CONTEXTFRAMEWORK_API FContextTagEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
	bool bRemoveOnEnd = true;
};

UENUM(BlueprintType)
enum class EContextLifetimePolicy : uint8
{
	Manual,
	RemoveOnEnterEnd
};


UCLASS(Blueprintable)
class CONTEXTFRAMEWORK_API UContextDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ContextTag;

	UPROPERTY(EditDefaultsOnly)
	TArray<FContextTagEntry> StateTags;
	
	UPROPERTY(EditDefaultsOnly)
	EContextLifetimePolicy LifetimePolicy = EContextLifetimePolicy::Manual;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer BlockedActions;

	UPROPERTY(EditDefaultsOnly)
	float MoveSpeedMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bCanBeInterrupted = true;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EnterMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EndMontage;

	UPROPERTY(EditDefaultsOnly)
	FVector TargetCameraOrActorOffset;

	UPROPERTY(EditDefaultsOnly)
	bool bLockMovement = true;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UContextCameraSetupDataAsset> CameraSetupData = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UContextInputMappingDataAsset> InputMappingData = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Priority = 0;
};

