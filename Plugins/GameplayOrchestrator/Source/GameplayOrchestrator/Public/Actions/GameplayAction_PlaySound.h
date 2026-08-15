// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GameplayActionBase.h"
#include "GameplayAction_PlaySound.generated.h"

class USoundBase;
/**
 * 
 */
UCLASS()
class GAMEPLAYORCHESTRATOR_API UGameplayAction_PlaySound : public UGameplayActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Play Sound")
	TObjectPtr<USoundBase> Sound;
 
	/** If true, plays at Context.Sender's location (3D, spatialized). If false, plays as a 2D UI-style sound. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Play Sound")
	bool bAtSenderLocation = true;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Play Sound", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.0f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Play Sound", meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.0f;
 
	virtual void Execute_Implementation(const FEventContext& Context) override;
 
#if WITH_EDITOR
	virtual FString GetEditorSummary() const override
	{
		return FString::Printf(TEXT("Play Sound: %s"), Sound ? *Sound->GetName() : TEXT("(none)"));
	}
#endif
};
