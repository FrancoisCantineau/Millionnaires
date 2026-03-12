#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ShipData.generated.h"



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
