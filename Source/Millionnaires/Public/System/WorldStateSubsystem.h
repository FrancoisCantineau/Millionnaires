// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveDataInterface.h"
#include "Data/WorldData.h"
#include "Map/BaseMapGenerator.h"
#include "Map/BaseRoomData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldStateSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UWorldStateSubsystem : public UGameInstanceSubsystem, public ISaveDataInterface
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FTrainState Train;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FSavedRoomData> SpawnedRooms;

	UFUNCTION(BlueprintCallable, Category = "Generator|Rooms")
	void CaptureMapFromGenerator(ABaseMapGenerator* Generator);

	virtual void OnSave(UMillionnaireSaveGame* SaveGame) override;
	virtual void OnLoad(UMillionnaireSaveGame* LoadGame) override;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
};


