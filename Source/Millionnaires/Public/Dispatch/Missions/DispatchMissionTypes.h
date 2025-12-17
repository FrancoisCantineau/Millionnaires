/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionTypes" - Header
 * Notes: Shared enums/structs for Dispatch mission system.
 */
#pragma once

#include "CoreMinimal.h"
#include "DispatchMissionTypes.generated.h"

#pragma region ENUMS

/** Mission mode: AI-only dispatch vs player-controlled FPS run (future). */
UENUM(BlueprintType)
enum class EDispatchMissionMode : uint8
{
    Dispatch UMETA(DisplayName="Dispatch Mission"),
    FPS      UMETA(DisplayName="FPS Mission"),
};

/** State of an active mission. */
UENUM(BlueprintType)
enum class EDispatchMissionState : uint8
{
    None       UMETA(DisplayName="None"),
    Accepted   UMETA(DisplayName="Accepted"),
    Traveling  UMETA(DisplayName="Traveling"),
    Working    UMETA(DisplayName="Working"),
    Returning  UMETA(DisplayName="Returning"),
    Completed  UMETA(DisplayName="Completed"),
    Failed     UMETA(DisplayName="Failed"),
    Aborted    UMETA(DisplayName="Aborted"),
};

/**
 * Mission location inside the spaceship. This is the shared "dropdown list"
 * used both by Mission Definitions (DataAssets) and Mission Sites (Actors).
 */
UENUM(BlueprintType)
enum class EDispatchMissionLocation : uint8
{
    Any           UMETA(DisplayName="Any"),
    Bridge        UMETA(DisplayName="Bridge"),
    Engineering   UMETA(DisplayName="Engineering"),
    ReactorCore   UMETA(DisplayName="Reactor Core"),
    MedBay        UMETA(DisplayName="MedBay"),
    CargoBay      UMETA(DisplayName="Cargo Bay"),
    Hangar        UMETA(DisplayName="Hangar"),
    CrewQuarters  UMETA(DisplayName="Crew Quarters"),
    Hydroponics   UMETA(DisplayName="Hydroponics"),
    Security      UMETA(DisplayName="Security"),
    AICore        UMETA(DisplayName="AI Core"),
};

#pragma endregion ENUMS

#pragma region STRUCTS

/** Difficulty bars on 10 (rolled per offer). */
USTRUCT(BlueprintType)
struct FDispatchMissionDifficulty
{
    GENERATED_BODY()

    /** Chance (0..10) to encounter a monster during travel. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 monsterChance10 = 0;

    /** Chance (0..10) to find loot during travel. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 lootChance10 = 0;

    /** Chance (0..10) that an extra complication occurs. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 complicationChance10 = 0;
};

#pragma endregion STRUCTS
