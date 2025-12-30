/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionNotificationPoint" - Header
 * Notes: World anchor used to spawn a mission offer UI indicator at a chosen location.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionNotificationPoint.generated.h"

/**
 * Notification point placed in the map.
 * World indicators (UMG) spawn near it when a mission offer appears at the same location.
 */
UCLASS()
class MILLIONNAIRES_API ADispatchMissionNotificationPoint : public ATargetPoint
{
    GENERATED_BODY()

public:
#pragma region SETTINGS

    /// <summary>Mission location this point is associated with.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|NotificationPoint")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /// <summary>Local widget offset (typically +Z).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|NotificationPoint")
    FVector widgetOffset = FVector(0.f, 0.f, 75.f);

#pragma endregion SETTINGS

public:
#pragma region API

    /// <summary>Returns the mission location.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|NotificationPoint")
    EDispatchMissionLocation GetMissionLocation() const { return missionLocation; }

    /// <summary>Returns the offset used when spawning the indicator.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Map|NotificationPoint")
    FVector GetWidgetOffset() const { return widgetOffset; }

#pragma endregion API
};
