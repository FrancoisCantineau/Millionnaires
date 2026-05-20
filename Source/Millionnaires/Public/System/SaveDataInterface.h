// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MillionnaireSaveGame.h"
#include "SaveDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MILLIONNAIRES_API ISaveDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void OnSave(UMillionnaireSaveGame* SaveGame) = 0;
	virtual void OnLoad(UMillionnaireSaveGame* LoadGame) = 0;
	
};
