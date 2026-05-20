// Fill out your copyright notice in the Description page of Project Settings.


#include "System/WorldStateSubsystem.h"

#include "System/MillionnaireSaveManager.h"

void UWorldStateSubsystem::CaptureMapFromGenerator(ABaseMapGenerator* Generator)
{
	
	SpawnedRooms.Empty();

	for (int32 FloorIndex = 0; FloorIndex< Generator->Floors.Num(); FloorIndex++)
	{
		for (ABaseRoomData* Room : Generator->GetSpawnedRooms(FloorIndex))
		{
			FSavedRoomData RoomData;
			
			RoomData.RoomAsset = Room->SourceDataAsset;
			RoomData.Transform = Room->OwningLevel->LevelTransform;
			SpawnedRooms.Add(RoomData);
		}
	}
	
}

void UWorldStateSubsystem::OnSave(UMillionnaireSaveGame* SaveGame)
{
	SaveGame->SpawnedRooms = SpawnedRooms;
	SaveGame->Train = Train;
}

void UWorldStateSubsystem::OnLoad(UMillionnaireSaveGame* LoadGame)
{
	SpawnedRooms = LoadGame->SpawnedRooms;
	Train = LoadGame->Train;
}

inline void UWorldStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UMillionnaireSaveManager* SaveManager = GetGameInstance()->GetSubsystem<UMillionnaireSaveManager>();
	SaveManager->RegisterSystem(this);
}