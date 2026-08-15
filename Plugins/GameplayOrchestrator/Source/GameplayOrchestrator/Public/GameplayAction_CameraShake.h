// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_CameraShake.generated.h"


class UCameraShakeBase;
/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayAction_CameraShake : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> ShakeClass;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Shake", meta = (ClampMin = "0.0"))
	float Scale = 1.0f;
 
	virtual void Execute_Implementation(const FEventContext& Context) override;
 
#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Camera Shake: %s"), ShakeClass ? *ShakeClass->GetName() : TEXT("(none)"));
	}
#endif
	
};
