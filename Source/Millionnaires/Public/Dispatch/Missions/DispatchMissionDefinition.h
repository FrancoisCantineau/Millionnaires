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
 * Defines what a mission can be: rules, difficulty ranges and possible rewards.
 * Mission offers are generated from this definition with a random seed.
 */
UCLASS(BlueprintType)
class MILLIONNAIRES_API UDispatchMissionDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
#pragma region GENERAL

    /** Mission display title. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    FText title;

    /** Mission type (Dispatch = AI, FPS = player-controlled future). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General")
    EDispatchMissionType missionType = EDispatchMissionType::Dispatch;

    /** Optional mission description shown in UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|General", meta=(MultiLine="true"))
    FText description;

#pragma endregion GENERAL

#pragma region DISPATCH_RULES

    /** Minimum agents required for Dispatch missions. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Dispatch", meta=(ClampMin="1"))
    int32 minAgents = 1;

    /** Maximum agents allowed for Dispatch missions. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Dispatch", meta=(ClampMin="1"))
    int32 maxAgents = 3;

    /** Travel speed used for simulation (cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Dispatch", meta=(ClampMin="1.0"))
    float travelSpeedCmPerSec = 400.f;

    /** Return speed used for simulation (cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Dispatch", meta=(ClampMin="1.0"))
    float returnSpeedCmPerSec = 450.f;

#pragma endregion DISPATCH_RULES

#pragma region FPS_RULES

    /** For FPS missions, only one agent is allowed (enforced by manager). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|FPS")
    bool bFpsSingleAgentOnly = true;

#pragma endregion FPS_RULES

#pragma region DIFFICULTY

    /** Ranges (0..10) used to roll offer difficulty. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty")
    FDispatchDifficultyRanges10 difficultyRanges;

#pragma endregion DIFFICULTY

#pragma region LOOT

    /** Loot pool used when rolling rewards (generic). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot")
    TArray<FDispatchLootEntry> lootPool;

    /** Max number of loot entries to roll. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Loot", meta=(ClampMin="0"))
    int32 maxLootRolls = 2;

#pragma endregion LOOT
};
