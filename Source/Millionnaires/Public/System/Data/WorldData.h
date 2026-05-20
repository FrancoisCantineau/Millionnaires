#pragma once

#include "CoreMinimal.h"
#include "WorldData.generated.h"


class URoomDataAsset;

USTRUCT(BlueprintType)
struct FTrainState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float Distance;

    UPROPERTY(BlueprintReadWrite)
    bool bIsMoving;

    UPROPERTY(BlueprintReadWrite)
    int32 CurrentLine;
    
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentStop; 
    
};

USTRUCT(BlueprintType)
struct FSavedRoomData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FTransform Transform;

    UPROPERTY(BlueprintReadWrite)
    URoomDataAsset* RoomAsset = nullptr;
};
