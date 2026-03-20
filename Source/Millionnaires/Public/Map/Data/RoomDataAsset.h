// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipData.h"
#include "Engine/DataAsset.h"
#include "RoomDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API URoomDataAsset : public UDataAsset
{
	GENERATED_BODY()

public :
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rooms")
	FRoomStruct RoomDataStruct;
};
