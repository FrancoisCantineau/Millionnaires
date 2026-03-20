// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/GeneratorSettingsDataAsset.h"
#include "Data/ShipData.h"
#include "GameFramework/Actor.h"
#include "BaseMapGenerator.generated.h"


USTRUCT(BlueprintType)
struct FRoomCacheEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 RemainingCount = 0;

	UPROPERTY(BlueprintReadWrite)
	float Weight = 1.0f;
};

UCLASS()
class MILLIONNAIRES_API ABaseMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Floors")
	UGeneratorSettingsDataAsset* GeneratorSettingsDataAsset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Generator|Floors")
	TArray<FFloorData> Floors;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	URoomDataAsset* CurrentlyTestedRoomType;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	TMap<URoomDataAsset*, FRoomCacheEntry> AvailableRoomsType;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere)
	TMap<URoomDataAsset*, FRoomCacheEntry> FailedAvailableRoomsType;

	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void InitFloorFromSettings(int32 FloorIndex);

	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void SetFirstRoom(int32 FloorIndex, ABaseRoomData* Room);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Floors")
	ABaseRoomData* GetFirstRoom(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Rooms")
	void AddRoomToZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoomData* Room);
 
	UFUNCTION(BlueprintCallable, Category = "Generator|Rooms")
	void RemoveRoomFromZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoomData* Room);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	TArray<ABaseRoomData*> GetRoomsInZone(int32 FloorIndex, FGameplayTag Zone) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	int32 GetRoomCountInZone(int32 FloorIndex, FGameplayTag Zone) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	TArray<ABaseRoomData*> GetSpawnedRooms(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Zones")
	void DecrementAvailableZone(int32 FloorIndex, FGameplayTag Zone);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Zones")
	TArray<FGameplayTag> GetAvailableZones(int32 FloorIndex) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Zones")
	bool IsZoneAvailable(int32 FloorIndex, FGameplayTag Zone) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Utils")
	bool IsFloorValid(int32 FloorIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Connections")
	void AddConnectionPair(int32 FloorIndex, ABaseDoorSlot* NewDoor, ABaseDoorSlot* AttachedDoor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Connections")
	ABaseDoorSlot* GetAttachedDoor(int32 FloorIndex, ABaseDoorSlot* Door) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Connections")
	void RemoveConnectionPair(int32 FloorIndex, ABaseDoorSlot* Door);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Connections")
	bool IsDoorConnected(int32 FloorIndex, ABaseDoorSlot* Door) const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void ClearFloor(int32 FloorIndex);

	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void ClearZone(int32 FloorIndex, FGameplayTag Zone);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Helpers")
	FFloorSettingsStruct GetFloorSettings(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Helpers")
	URoomDataAsset* GetRandomRoomForZone(int32 FloorIndex, FGameplayTag Zone);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Helpers")
	bool CanSpawnRoom(int32 FloorIndex, URoomDataAsset* Room) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Helpers")
	void IncrementRoomSpawnCount(int32 FloorIndex, URoomDataAsset* Room);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Zones")
	bool CanAddRoomToZone(int32 FloorIndex, FGameplayTag Zone) const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	void BuildZoneCandidates(int32 FloorIndex, FGameplayTag Zone);
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	URoomDataAsset* PopRandomRoomFromCache();
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	void FailRoomPlacement(URoomDataAsset* Room);

	UFUNCTION(BlueprintCallable, Category = "Generator|Helpers")
	ABaseRoomData* GetRoomDataFromLevel(ULevelStreamingDynamic* Level);
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	void RestoreFailedCandidates();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Cache")
	bool IsCacheEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	URoomDataAsset* GetWeightedRandomRoom() const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Doors")
	void AddRoomDoorsToFree(int32 FloorIndex, ABaseRoomData* RoomData);
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Doors")
	void RemoveDoorFromFree(int32 FloorIndex, ABaseDoorSlot* Door);
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Doors")
	ABaseDoorSlot* GetRandomFreeDoor(int32 FloorIndex,FGameplayTag Zone) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Doors")
	TArray<ABaseDoorSlot*> GetFreeDoors(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Doors")
	bool HasFreeDoors(int32 FloorIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Cache")
	void DecrementRoomFromCache(URoomDataAsset* Room);

	
private:
	FFloorData* GetFloorData(int32 FloorIndex);
	const FFloorData* GetFloorData(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void InitFloor(int32 FloorIndex, TMap<FGameplayTag, int32> ZoneLimits);

	
};
