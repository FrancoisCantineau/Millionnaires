// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ShipData.h"
#include "GameFramework/Actor.h"
#include "BaseMapGenerator.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Generator|Floors")
	TArray<FFloorData> Floors;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void InitFloor(int32 FloorIndex, TMap<FGameplayTag, int32> ZoneLimits);

	UFUNCTION(BlueprintCallable, Category = "Generator|Floors")
	void SetFirstRoom(int32 FloorIndex, ABaseRoom* Room);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Floors")
	ABaseRoom* GetFirstRoom(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Rooms")
	void AddRoomToZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoom* Room);
 
	UFUNCTION(BlueprintCallable, Category = "Generator|Rooms")
	void RemoveRoomFromZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoom* Room);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	TArray<ABaseRoom*> GetRoomsInZone(int32 FloorIndex, FGameplayTag Zone) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	int32 GetRoomCountInZone(int32 FloorIndex, FGameplayTag Zone) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Rooms")
	TArray<AActor*> GetSpawnedRooms(int32 FloorIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Zones")
	void DecrementAvailableZone(int32 FloorIndex, FGameplayTag Zone);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Zones")
	TArray<FGameplayTag> GetAvailableZones(int32 FloorIndex) const;
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Zones")
	bool IsZoneAvailable(int32 FloorIndex, FGameplayTag Zone) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Utils")
	bool IsFloorValid(int32 FloorIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Generator|Connections")
	void AddConnectionPair(int32 FloorIndex, ADoorBaseArrow* NewDoor, ADoorBaseArrow* AttachedDoor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Connections")
	ADoorBaseArrow* GetAttachedDoor(int32 FloorIndex, ADoorBaseArrow* Door) const;
	
	UFUNCTION(BlueprintCallable, Category = "Generator|Connections")
	void RemoveConnectionPair(int32 FloorIndex, ADoorBaseArrow* Door);
 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generator|Connections")
	bool IsDoorConnected(int32 FloorIndex, ADoorBaseArrow* Door) const;
	
private:
	FFloorData* GetFloorData(int32 FloorIndex);
	const FFloorData* GetFloorData(int32 FloorIndex) const;
	
};
