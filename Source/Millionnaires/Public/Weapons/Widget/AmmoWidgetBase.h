// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UAmmoWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateAmmos(int m_CurrentAmmos, int m_RemainingAmmos);
};
