#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ShipData.generated.h"


class ADoorBaseArrow;
class ABaseRoom;

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Room        UMETA(DisplayName = "Room"),
	Hallway     UMETA(DisplayName = "Hallway")
};
USTRUCT(BlueprintType)
struct FRoomStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	TSubclassOf<AActor> RoomClass;
 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	ERoomType RoomType = ERoomType::Room;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	TSubclassOf<AActor> MiniVersion;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 Floor = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	AActor* RoomRef = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	FGameplayTag ZoneType;
};


USTRUCT(BlueprintType)
struct FFloorSettings
{
	GENERATED_BODY()
 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	int32 MaxRoomAmount = 10;
 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	FRoomStruct RoomTypeAllowed;
 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	TMap<FGameplayTag, int32> ZoneType;
};

USTRUCT(BlueprintType)
struct FZoneData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<ABaseRoom*> Rooms;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxRooms = 0;
};

USTRUCT(BlueprintType)
struct FFloorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ABaseRoom* FirstRoom = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, FZoneData> ZoneData;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameplayTag> AvailableZonesLeft;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> SpawnedRooms;

	UPROPERTY(BlueprintReadWrite)
	TMap<ADoorBaseArrow*, ADoorBaseArrow*> ConnectionPairs;
};

