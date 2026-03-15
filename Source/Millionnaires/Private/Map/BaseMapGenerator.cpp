// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/BaseMapGenerator.h"
#include "Map/BaseRoom.h"
#include "Map/DoorBaseArrow.h"


// Sets default values
ABaseMapGenerator::ABaseMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FFloorData* ABaseMapGenerator::GetFloorData(int32 FloorIndex)
{
    if (!IsFloorValid(FloorIndex)) return nullptr;
    return &Floors[FloorIndex];
}
 
const FFloorData* ABaseMapGenerator::GetFloorData(int32 FloorIndex) const
{
    if (!IsFloorValid(FloorIndex)) return nullptr;
    return &Floors[FloorIndex];
}
 
bool ABaseMapGenerator::IsFloorValid(int32 FloorIndex) const
{
    return Floors.IsValidIndex(FloorIndex);
}


void ABaseMapGenerator::InitFloor(int32 FloorIndex, TMap<FGameplayTag, int32> ZoneLimits)
{
    while (!Floors.IsValidIndex(FloorIndex))
    {
        Floors.Add(FFloorData());
    }

    FFloorData& Floor = Floors[FloorIndex];
    
    if (Floor.AvailableZonesLeft.Num() > 0) return;

    for (auto& Pair : ZoneLimits)
    {
        FZoneData& ZoneData = Floor.ZoneData.FindOrAdd(Pair.Key);
        ZoneData.MaxRooms = Pair.Value;
        Floor.AvailableZonesLeft.AddUnique(Pair.Key);
    }
}

void ABaseMapGenerator::SetFirstRoom(int32 FloorIndex, ABaseRoom* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    Floor->FirstRoom = Room;
}

ABaseRoom* ABaseMapGenerator::GetFirstRoom(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return nullptr;
 
    return Floor->FirstRoom;
}

void ABaseMapGenerator::AddRoomToZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoom* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    FZoneData& ZoneData = Floor->ZoneData.FindOrAdd(Zone);
    ZoneData.Rooms.AddUnique(Room);
 
    Floor->SpawnedRooms.AddUnique(Room);
}
 
void ABaseMapGenerator::RemoveRoomFromZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoom* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return;
 
    ZoneData->Rooms.Remove(Room);
    Floor->SpawnedRooms.Remove(Room);
}
 
TArray<ABaseRoom*> ABaseMapGenerator::GetRoomsInZone(int32 FloorIndex, FGameplayTag Zone) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return {};
 
    const FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return {};
 
    return ZoneData->Rooms;
}
 
int32 ABaseMapGenerator::GetRoomCountInZone(int32 FloorIndex, FGameplayTag Zone) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return 0;
 
    const FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return 0;
 
    return ZoneData->Rooms.Num();
}
 
TArray<AActor*> ABaseMapGenerator::GetSpawnedRooms(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return {};
 
    return Floor->SpawnedRooms;
}

void ABaseMapGenerator::DecrementAvailableZone(int32 FloorIndex, FGameplayTag Zone)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return;
 
    Floor->AvailableZonesLeft.Remove(Zone);
}
 
TArray<FGameplayTag> ABaseMapGenerator::GetAvailableZones(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return {};
 
    return Floor->AvailableZonesLeft;
}
 
bool ABaseMapGenerator::IsZoneAvailable(int32 FloorIndex, FGameplayTag Zone) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return false;
 
    return Floor->AvailableZonesLeft.Contains(Zone);
}
 

void ABaseMapGenerator::AddConnectionPair(int32 FloorIndex, ADoorBaseArrow* NewDoor, ADoorBaseArrow* AttachedDoor)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(NewDoor) || !IsValid(AttachedDoor)) return;
 
    Floor->ConnectionPairs.Add(NewDoor, AttachedDoor);
}

ADoorBaseArrow* ABaseMapGenerator::GetAttachedDoor(int32 FloorIndex, ADoorBaseArrow* Door) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return nullptr;
 
    ADoorBaseArrow* const* AttachedDoor = Floor->ConnectionPairs.Find(Door);
    return AttachedDoor ? *AttachedDoor : nullptr;
}

void ABaseMapGenerator::RemoveConnectionPair(int32 FloorIndex, ADoorBaseArrow* Door)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return;
 
    Floor->ConnectionPairs.Remove(Door);
}

bool ABaseMapGenerator::IsDoorConnected(int32 FloorIndex, ADoorBaseArrow* Door) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return false;
 
    return Floor->ConnectionPairs.Contains(Door);
}

void ABaseMapGenerator::ClearFloor(int32 FloorIndex)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return;

    for (AActor* Room : Floor->SpawnedRooms)
    {
        if (!IsValid(Room) || Room == Floor->FirstRoom) continue;
        Room->Destroy();
    }
    
    ABaseRoom* SavedFirstRoom = Floor->FirstRoom;
    *Floor = FFloorData();
    Floor->FirstRoom = SavedFirstRoom;
    
    if (IsValid(SavedFirstRoom))
    {
        Floor->SpawnedRooms.Add(SavedFirstRoom);
    }
}

void ABaseMapGenerator::ClearZone(int32 FloorIndex, FGameplayTag Zone)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return;

    FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return;
    
    for (ABaseRoom* Room : ZoneData->Rooms)
    {
        if (IsValid(Room))
        {
            Floor->SpawnedRooms.Remove(Room);
            Room->Destroy();
        }
    }
    
    int32 Max = ZoneData->MaxRooms;
    *ZoneData = FZoneData();
    ZoneData->MaxRooms = Max;
    
    Floor->AvailableZonesLeft.AddUnique(Zone);
}
