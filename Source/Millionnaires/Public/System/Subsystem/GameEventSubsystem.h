// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SharedType/GameEvent.h"
#include "GameEventSubsystem.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FGameEventSignature, const FGameEvent&);

UCLASS()
class MILLIONNAIRES_API UGameEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	FGameEventSignature OnEvent;

	void EmitEvent(const FGameEvent& Event);
	
};
