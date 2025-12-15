/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMapBoundsVolume" - Header
 * Notes: Defines world bounds used to convert world locations to normalized map positions.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DispatchMapBoundsVolume.generated.h"

class UBoxComponent;

/**
 * Place this actor in your Dispatch level to define the world area represented by the map UI.
 * Mission offer world locations are converted into normalized map coords using this box.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchMapBoundsVolume : public AActor
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    ADispatchMapBoundsVolume();

#pragma endregion LIFECYCLE

#pragma region API

    /** Returns the min world of the box (XY is used for mapping). */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map")
    FVector GetWorldMin() const;

    /** Returns the max world of the box (XY is used for mapping). */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map")
    FVector GetWorldMax() const;

    /** Returns world center of the box. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Map")
    FVector GetWorldCenter() const;

#pragma endregion API

protected:
#pragma region COMPONENTS

    /** Box representing the world area covered by the map. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Map", meta=(AllowPrivateAccess="true"))
    UBoxComponent* boundsBox = nullptr;

#pragma endregion COMPONENTS
};
