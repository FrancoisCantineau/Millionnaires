// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/BaseMapGenerator.h"
#include "Map/BaseRoomData.h"
#include "Map/Data/RoomDataAsset.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Map/BaseDoorSlot.h"


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

void ABaseMapGenerator::InitFloorFromSettings(int32 FloorIndex)
{
    FFloorSettingsStruct Settings = GetFloorSettings(FloorIndex);

    TMap<FGameplayTag, int32> ZoneLimits;
    for (auto& Pair : Settings.ZoneSetup)
    {
        ZoneLimits.Add(Pair.Key, Pair.Value.MaxZoneRooms);
    }

    InitFloor(FloorIndex, ZoneLimits);
}

void ABaseMapGenerator::SetFirstRoom(int32 FloorIndex, ABaseRoomData* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    Floor->FirstRoom = Room;
}

ABaseRoomData* ABaseMapGenerator::GetFirstRoom(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return nullptr;
 
    return Floor->FirstRoom;
}

void ABaseMapGenerator::AddRoomToZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoomData* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    FZoneData& ZoneData = Floor->ZoneData.FindOrAdd(Zone);
    ZoneData.Rooms.AddUnique(Room);
 
    Floor->SpawnedRooms.AddUnique(Room);
}
 
void ABaseMapGenerator::RemoveRoomFromZone(int32 FloorIndex, FGameplayTag Zone, ABaseRoomData* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return;
 
    ZoneData->Rooms.Remove(Room);
    Floor->SpawnedRooms.Remove(Room);
}
 
TArray<ABaseRoomData*> ABaseMapGenerator::GetRoomsInZone(int32 FloorIndex, FGameplayTag Zone) const
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

TArray<ABaseRoomData*> ABaseMapGenerator::GetSpawnedRooms(int32 FloorIndex) const
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
 

void ABaseMapGenerator::AddConnectionPair(int32 FloorIndex, ABaseDoorSlot* NewDoor, ABaseDoorSlot* AttachedDoor)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(NewDoor) || !IsValid(AttachedDoor)) return;
 
    Floor->ConnectionPairs.Add(NewDoor, AttachedDoor);
}

ABaseDoorSlot* ABaseMapGenerator::GetAttachedDoor(int32 FloorIndex, ABaseDoorSlot* Door) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return nullptr;
 
    ABaseDoorSlot* const* AttachedDoor = Floor->ConnectionPairs.Find(Door);
    return AttachedDoor ? *AttachedDoor : nullptr;
}

void ABaseMapGenerator::RemoveConnectionPair(int32 FloorIndex, ABaseDoorSlot* Door)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return;
 
    Floor->ConnectionPairs.Remove(Door);
}

bool ABaseMapGenerator::IsDoorConnected(int32 FloorIndex, ABaseDoorSlot* Door) const
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
    
    ABaseRoomData* SavedFirstRoom = Floor->FirstRoom;
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
    
    for (ABaseRoomData* Room : ZoneData->Rooms)
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

FFloorSettingsStruct ABaseMapGenerator::GetFloorSettings(int32 FloorIndex) const
{
    if (!GeneratorSettingsDataAsset) return FFloorSettingsStruct();
    if (!GeneratorSettingsDataAsset->FloorSettings.IsValidIndex(FloorIndex)) return FFloorSettingsStruct();
 
    return GeneratorSettingsDataAsset->FloorSettings[FloorIndex];
}

URoomDataAsset* ABaseMapGenerator::GetRandomRoomForZone(int32 FloorIndex, FGameplayTag Zone)
{
    FFloorSettingsStruct Settings = GetFloorSettings(FloorIndex);
    
    TArray<URoomDataAsset*> Candidates;
 
    const FZoneEntry* ZoneEntry = Settings.ZoneSetup.Find(Zone);
    if (!ZoneEntry) return nullptr;
 
    for (const FRoomEntry& Entry : ZoneEntry->RoomPool)
    {
        if (!IsValid(Entry.RoomData)) continue;
        if (CanSpawnRoom(FloorIndex, Entry.RoomData))
        {
            Candidates.Add(Entry.RoomData);
        }
    }
 
    if (Candidates.IsEmpty()) return nullptr;
    return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
}

bool ABaseMapGenerator::CanSpawnRoom(int32 FloorIndex, URoomDataAsset* Room) const
{
    if (!IsValid(Room)) return false;
 
    FFloorSettingsStruct Settings = GetFloorSettings(FloorIndex);
    
    for (auto& ZonePair : Settings.ZoneSetup)
    {
        for (const FRoomEntry& Entry : ZonePair.Value.RoomPool)
        {
            if (Entry.RoomData != Room) continue;
            
            if (Entry.MaxOccurrences == 0) return true;
            
            const FFloorData* Floor = GetFloorData(FloorIndex);
            if (!Floor) return true;
 
            const int32* Count = Floor->RoomSpawnCounts.Find(Room);
            int32 CurrentCount = Count ? *Count : 0;
 
            return CurrentCount < Entry.MaxOccurrences;
        }
    }
 
    return false;
}

void ABaseMapGenerator::IncrementRoomSpawnCount(int32 FloorIndex, URoomDataAsset* Room)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Room)) return;
 
    int32& Count = Floor->RoomSpawnCounts.FindOrAdd(Room);
    Count++;
}

bool ABaseMapGenerator::CanAddRoomToZone(int32 FloorIndex, FGameplayTag Zone) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return false;
 
    const FZoneData* ZoneData = Floor->ZoneData.Find(Zone);
    if (!ZoneData) return false;
 
    return ZoneData->Rooms.Num() < ZoneData->MaxRooms;
}

void ABaseMapGenerator::BuildZoneCandidates(int32 FloorIndex, FGameplayTag Zone)
{
    AvailableRoomsType.Empty();
    FailedAvailableRoomsType.Empty();

    FFloorSettingsStruct Settings = GetFloorSettings(FloorIndex);

    if (!Zone.IsValid())
    {
        for (auto& ZonePair : Settings.ZoneSetup)
        {
            for (const FRoomEntry& Entry : ZonePair.Value.RoomPool)
            {
                if (!IsValid(Entry.RoomData)) continue;
                int32 Max = Entry.MaxOccurrences == 0 ? 1 : Entry.MaxOccurrences;
                FRoomCacheEntry& CacheEntry = AvailableRoomsType.FindOrAdd(Entry.RoomData);
                CacheEntry.RemainingCount += Max;
                CacheEntry.Weight = Entry.Weight;
            }
        }
        return;
    }

    FZoneEntry* ZoneEntry = Settings.ZoneSetup.Find(Zone);
    if (!ZoneEntry) return;

    for (const FRoomEntry& Entry : ZoneEntry->RoomPool)
    {
        if (!IsValid(Entry.RoomData)) continue;
        int32 Max = Entry.MaxOccurrences == 0 ? 1 : Entry.MaxOccurrences;
        FRoomCacheEntry& CacheEntry = AvailableRoomsType.FindOrAdd(Entry.RoomData);
        CacheEntry.RemainingCount = Max;
        CacheEntry.Weight = Entry.Weight;
    }
}

URoomDataAsset* ABaseMapGenerator::PopRandomRoomFromCache()
{
    if (AvailableRoomsType.IsEmpty()) return nullptr;
    
    TArray<URoomDataAsset*> Keys;
    AvailableRoomsType.GetKeys(Keys);
    
    URoomDataAsset* Room = Keys[FMath::RandRange(0, Keys.Num() - 1)];
    return Room;
}

void ABaseMapGenerator::FailRoomPlacement(URoomDataAsset* Room)
{
    if (!IsValid(Room)) return;

    FRoomCacheEntry* Entry = AvailableRoomsType.Find(Room);
    if (!Entry) return;

    FailedAvailableRoomsType.FindOrAdd(Room) = *Entry;
    AvailableRoomsType.Remove(Room);
}

ABaseRoomData* ABaseMapGenerator::GetRoomDataFromLevel(ULevelStreamingDynamic* Level)
{
    if (!IsValid(Level)) return nullptr;

    ULevel* LoadedLevel = Level->GetLoadedLevel();
    if (!LoadedLevel) return nullptr;

    for (AActor* Actor : LoadedLevel->Actors)
    {
        ABaseRoomData* RoomData = Cast<ABaseRoomData>(Actor);
        UE_LOG(LogTemp, Warning, TEXT("Picked RoomData: %s (%p)"),
    *GetNameSafe(RoomData),
    RoomData);
        if (IsValid(RoomData)) return RoomData;
        
    }

    return nullptr;
}

void ABaseMapGenerator::RestoreFailedCandidates()
{
    for (auto& Pair : FailedAvailableRoomsType)
    {
        AvailableRoomsType.FindOrAdd(Pair.Key) = Pair.Value;
    }
    FailedAvailableRoomsType.Empty();
}

bool ABaseMapGenerator::IsCacheEmpty() const
{
    return AvailableRoomsType.IsEmpty();
}

URoomDataAsset* ABaseMapGenerator::GetWeightedRandomRoom() const
{
    if (AvailableRoomsType.IsEmpty()) return nullptr;
    
    float TotalWeight = 0.f;
    for (auto& Pair : AvailableRoomsType)
        TotalWeight += Pair.Value.Weight;
    
    float Roll = FMath::FRandRange(0.f, TotalWeight);
    
    float Cumulative = 0.f;
    for (auto& Pair : AvailableRoomsType)
    {
        Cumulative += Pair.Value.Weight;
        if (Roll <= Cumulative)
            return Pair.Key;
    }
 
    return nullptr;
}

void ABaseMapGenerator::AddRoomDoorsToFree(int32 FloorIndex, ABaseRoomData* RoomData)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(RoomData)) return;
 
    for (FDoorSlotData& Door : RoomData->GetFreeDoors())
    {
        if (IsValid(Door.Door))
        {
            Door.Door->ZoneType = RoomData->ZoneType;
            Door.Door->ParentRoom = RoomData;
            Floor->FreeDoors.AddUnique(Door.Door);
        }
    }
}

void ABaseMapGenerator::RemoveDoorFromFree(int32 FloorIndex, ABaseDoorSlot* Door)
{
    FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || !IsValid(Door)) return;
 
    Floor->FreeDoors.Remove(Door);
}

ABaseDoorSlot* ABaseMapGenerator::GetRandomFreeDoor(int32 FloorIndex,FGameplayTag Zone) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor || Floor->FreeDoors.IsEmpty()) return nullptr;

    if (!Zone.IsValid())
    {
        return Floor->FreeDoors[FMath::RandRange(0, Floor->FreeDoors.Num() - 1)];
    }
    
    TArray<ABaseDoorSlot*> ZoneDoors;
    for (ABaseDoorSlot* Door : Floor->FreeDoors)
    {
        if (!IsValid(Door)) continue;
        if (!IsValid(Door->ParentRoom)) continue;
        if (Door->ParentRoom->ZoneType == Zone)
            ZoneDoors.Add(Door);
    }
 
    if (ZoneDoors.IsEmpty()) return nullptr;
    return ZoneDoors[FMath::RandRange(0, ZoneDoors.Num() - 1)];
}

TArray<ABaseDoorSlot*> ABaseMapGenerator::GetFreeDoors(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return {};
    return Floor->FreeDoors;
}

bool ABaseMapGenerator::HasFreeDoors(int32 FloorIndex) const
{
    const FFloorData* Floor = GetFloorData(FloorIndex);
    if (!Floor) return false;
    return Floor->FreeDoors.Num() > 0;
}

void ABaseMapGenerator::DecrementRoomFromCache(URoomDataAsset* Room)
{
    if (!IsValid(Room)) return;

    FRoomCacheEntry* Entry = AvailableRoomsType.Find(Room);
    if (!Entry) return;

    Entry->RemainingCount--;
    
    if (Entry->RemainingCount <= 0)
        AvailableRoomsType.Remove(Room);
}
