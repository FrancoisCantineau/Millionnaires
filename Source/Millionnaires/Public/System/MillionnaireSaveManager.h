// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveDataInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MillionnaireSaveManager.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UMillionnaireSaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :

	UFUNCTION(BlueprintCallable, Category = "Save")
	void RegisterSystem(TScriptInterface<ISaveDataInterface> System);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveAll();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadAll();

private:
	TArray<TScriptInterface<ISaveDataInterface>> RegisteredSystems;
};
