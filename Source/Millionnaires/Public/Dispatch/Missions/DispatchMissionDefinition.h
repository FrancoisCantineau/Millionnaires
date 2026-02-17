/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionDefinition" - Header
 * Notes: DataAsset describing a mission archetype for Dispatch/FPS missions.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionDefinition.generated.h"

class UTexture2D;

/**
 * Mission Definition: defines what a mission is, how it spawns, where it can spawn,
 * and how long it stays available before expiring.
 */
UCLASS(BlueprintType)
class MILLIONNAIRES_API UDispatchMissionDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
#pragma region GENERAL

    /// <summary>Unique identifier for this mission definition.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    FName missionId = NAME_None;

    /// <summary>Title shown to the player.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    FText title;

    /// <summary>Short description.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General", meta=(MultiLine=true))
    FText description;

    /// <summary>Optional icon used by world notifications and mission details.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    TObjectPtr<UTexture2D> icon = nullptr;

    /// <summary>Mission mode (Dispatch now, FPS later).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /// <summary>Mission location dropdown (must match a Mission Site in the level).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

#pragma endregion GENERAL

#pragma region SUCCESS_MODEL

    /// <summary>Defines how success chance is computed when selecting characters.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    FDispatchMissionSuccessModel successModel;

#pragma endregion SUCCESS_MODEL


#pragma region TIME_LIMIT

    /// <summary>Offer time limit (seconds): if the player doesn't accept in time, the offer expires.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|TimeLimit", meta=(ClampMin="1.0"))
    float offerTimeLimitMinSec = 45.f;

    /// <summary>Offer time limit max (seconds). If equal to min, the time limit is fixed.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|TimeLimit", meta=(ClampMin="1.0"))
    float offerTimeLimitMaxSec = 90.f;

#pragma endregion TIME_LIMIT

#pragma region DIFFICULTY_RANGES

    /// <summary>Monster encounter chance range (0..10).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint monsterChanceRange10 = FIntPoint(1, 6);

    /// <summary>Loot chance range (0..10).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint lootChanceRange10 = FIntPoint(2, 8);

    /// <summary>Complication chance range (0..10).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint complicationChanceRange10 = FIntPoint(0, 5);

#pragma endregion DIFFICULTY_RANGES

#pragma region STAGE_DURATIONS

    /// <summary>Travel duration range (seconds).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D travelDurationRangeSec = FVector2D(6.f, 14.f);

    /// <summary>Work duration range (seconds).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D workDurationRangeSec = FVector2D(4.f, 10.f);

    /// <summary>Return duration range (seconds).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D returnDurationRangeSec = FVector2D(6.f, 14.f);

#pragma endregion STAGE_DURATIONS
};
