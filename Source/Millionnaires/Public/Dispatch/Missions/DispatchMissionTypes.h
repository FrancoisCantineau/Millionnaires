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
enum class EDispatchMissionType : uint8
{
    Dispatch UMETA(DisplayName="Dispatch Mission"),
    FPS      UMETA(DisplayName="FPS Mission"),
};

/** Mission lifecycle state. */
UENUM(BlueprintType)
enum class EDispatchMissionState : uint8
{
    Offered      UMETA(DisplayName="Offered"),
    Accepted     UMETA(DisplayName="Accepted"),
    Traveling    UMETA(DisplayName="Traveling"),
    Resolving    UMETA(DisplayName="Resolving"),
    Returning    UMETA(DisplayName="Returning"),
    Completed    UMETA(DisplayName="Completed"),
    Failed       UMETA(DisplayName="Failed"),
    Aborted      UMETA(DisplayName="Aborted"),
};

#pragma endregion ENUMS

#pragma region STRUCTS

/** Difficulty bars (0..10) rolled per mission offer. */
USTRUCT(BlueprintType)
struct FDispatchMissionDifficulty
{
    GENERATED_BODY()

    /** Chance (0..10) to meet a monster during the trip. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 monsterChance10 = 0;

    /** Chance (0..10) to find useful stuff during the trip. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 lootChance10 = 0;

    /** Chance (0..10) that an additional complication happens. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 complicationChance10 = 0;
};

/** Simple inclusive int range helper. */
USTRUCT(BlueprintType)
struct FDispatchIntRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 min = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 max = 0;

    int32 RandomInRange(FRandomStream& Rng) const
    {
        const int32 A = FMath::Min(min, max);
        const int32 B = FMath::Max(min, max);
        return Rng.RandRange(A, B);
    }
};

/** Difficulty bar ranges used by mission definitions to roll an offer. */
USTRUCT(BlueprintType)
struct FDispatchDifficultyRanges10
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty")
    FDispatchIntRange monsterChance10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty")
    FDispatchIntRange lootChance10;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty")
    FDispatchIntRange complicationChance10;
};

/** A weighted loot entry (generic, you can point it to your own item/definition assets). */
USTRUCT(BlueprintType)
struct FDispatchLootEntry
{
    GENERATED_BODY()

    /** Soft pointer to a loot asset (can be your own ItemDefinition / DataAsset). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot")
    TSoftObjectPtr<UObject> lootAsset;

    /** Roll count range. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot")
    FDispatchIntRange countRange;

    /** Weight used by weighted random selection. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot", meta=(ClampMin="0.0"))
    float weight = 1.f;
};

/** One rolled loot result. */
USTRUCT(BlueprintType)
struct FDispatchLootRoll
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot")
    TSoftObjectPtr<UObject> lootAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot")
    int32 count = 1;
};

#pragma endregion STRUCTS
