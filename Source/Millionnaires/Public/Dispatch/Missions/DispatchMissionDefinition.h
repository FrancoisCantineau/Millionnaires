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

    /** Unique identifier for this mission definition. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    FName missionId = NAME_None;

    /** Title shown to the player. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    FText title;

    /** Short description. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General", meta=(MultiLine=true))
    FText description;

    /** Mission mode (Dispatch now, FPS later). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /** Mission location dropdown (must match a Mission Site in the level). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

#pragma endregion GENERAL

#pragma region TIME_LIMIT

    /** Offer time limit (seconds): if the player doesn't accept in time, the offer expires and fails. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|TimeLimit", meta=(ClampMin="1.0"))
    float offerTimeLimitMinSec = 45.f;

    /** Offer time limit max (seconds). If equal to min, the time limit is fixed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|TimeLimit", meta=(ClampMin="1.0"))
    float offerTimeLimitMaxSec = 90.f;

#pragma endregion TIME_LIMIT

#pragma region DIFFICULTY_RANGES

    /** Monster encounter chance range (0..10). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint monsterChanceRange10 = FIntPoint(1, 6);

    /** Loot chance range (0..10). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint lootChanceRange10 = FIntPoint(2, 8);

    /** Complication chance range (0..10). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    FIntPoint complicationChanceRange10 = FIntPoint(0, 5);

#pragma endregion DIFFICULTY_RANGES

#pragma region STAGE_DURATIONS

    /** Travel duration range (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D travelDurationRangeSec = FVector2D(6.f, 14.f);

    /** Work duration range (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D workDurationRangeSec = FVector2D(4.f, 10.f);

    /** Return duration range (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Durations", meta=(ClampMin="0.1"))
    FVector2D returnDurationRangeSec = FVector2D(6.f, 14.f);

#pragma endregion STAGE_DURATIONS
};
