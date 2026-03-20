// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GeneratorSettingsDataAsset.generated.h"

/**
 * 
 */

class URoomDataAsset;

USTRUCT(BlueprintType)
struct FRoomEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	URoomDataAsset* RoomData = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 MaxOccurrences = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	float Weight = 1.0f;
};

USTRUCT(BlueprintType)
struct FZoneEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 MaxZoneRooms = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rooms")
	TArray<FRoomEntry> RoomPool;
};

USTRUCT(BlueprintType)
struct FFloorSettingsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	URoomDataAsset* FirstFloorRoom = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	int32 MaxRoomAmount = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	TMap<FGameplayTag, FZoneEntry> ZoneSetup;
};

UCLASS()
class MILLIONNAIRES_API UGeneratorSettingsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rooms")
	TArray<FFloorSettingsStruct> FloorSettings;
	
};
