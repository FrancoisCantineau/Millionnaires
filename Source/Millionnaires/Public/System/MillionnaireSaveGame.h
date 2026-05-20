// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/WorldData.h"
#include "GameFramework/SaveGame.h"
#include "MillionnaireSaveGame.generated.h"

struct FSavedRoomData;
/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UMillionnaireSaveGame : public USaveGame
{
	GENERATED_BODY()

public :
	
	UPROPERTY()
	TArray<FSavedRoomData> SpawnedRooms;
	
	UPROPERTY()
	FTrainState Train; 
	
};
