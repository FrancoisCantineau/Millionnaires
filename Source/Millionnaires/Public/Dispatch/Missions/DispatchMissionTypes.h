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

class UDispatchMissionDefinition;
class ADispatchMissionSiteActor;
class APawn;

#pragma region ENUMS

/// <summary>Mission mode: AI-only dispatch vs player-controlled FPS run (future).</summary>
UENUM(BlueprintType)
enum class EDispatchMissionMode : uint8
{
    Dispatch UMETA(DisplayName="Dispatch Mission"),
    FPS      UMETA(DisplayName="FPS Mission"),
};

/// <summary>State of an active mission.</summary>
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

/// <summary>Mission location inside the spaceship. Used by definitions, sites and notification points.</summary>
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

/// <summary>Click proxy kind. Used to open the mission UI from world clicks.</summary>
UENUM(BlueprintType)
enum class EDispatchMissionClickProxyKind : uint8
{
    None   UMETA(DisplayName="None"),
    Offer  UMETA(DisplayName="Offer"),
    Mission UMETA(DisplayName="Mission"),
};

#pragma endregion ENUMS

#pragma region STRUCTS

/// <summary>Difficulty bars on 10 (rolled per offer).</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionDifficulty
{
    GENERATED_BODY()

    /// <summary>Chance (0..10) to encounter a monster during travel.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 monsterChance10 = 0;

    /// <summary>Chance (0..10) to find loot during travel.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 lootChance10 = 0;

    /// <summary>Chance (0..10) that an extra complication occurs.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Difficulty", meta=(ClampMin="0", ClampMax="10"))
    int32 complicationChance10 = 0;
};

/// <summary>A mission offer generated at runtime from a definition.</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionOffer
{
    GENERATED_BODY()

    /// <summary>Unique offer id.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid offerId;

    /// <summary>Definition asset used to generate this offer.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<UDispatchMissionDefinition> definition = nullptr;

    /// <summary>Mission mode.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /// <summary>Mission location dropdown value.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /// <summary>Actor representing the mission site (building/zone group).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<ADispatchMissionSiteActor> locationActor = nullptr;

    /// <summary>World location where this mission happens.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FVector worldLocation = FVector::ZeroVector;

    /// <summary>Difficulty rolled from definition ranges.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FDispatchMissionDifficulty difficulty;

    /// <summary>Day when this offer was created.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 dayCreated = 1;

    /// <summary>Random seed used for loot & outcomes (deterministic if reused).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 seed = 0;

    /// <summary>Offer time limit rolled for this offer (seconds).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float timeLimitSec = 60.f;

    /// <summary>Remaining time before expiry (seconds).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float timeRemainingSec = 60.f;
};

/// <summary>Active mission in progress (after accepting an offer).</summary>
USTRUCT(BlueprintType)
struct FDispatchActiveMission
{
    GENERATED_BODY()

    /// <summary>Mission id.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid missionId;

    /// <summary>Offer id this mission came from.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid sourceOfferId;

    /// <summary>Definition.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<UDispatchMissionDefinition> definition = nullptr;

    /// <summary>Mission mode.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /// <summary>Mission location dropdown.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /// <summary>Actor representing the mission site.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<ADispatchMissionSiteActor> locationActor = nullptr;

    /// <summary>World location.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FVector worldLocation = FVector::ZeroVector;

    /// <summary>Assigned agents (AI pawns). FPS missions should contain exactly 1 agent (future).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TArray<TObjectPtr<APawn>> assignedAgents;

    /// <summary>Mission state.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionState state = EDispatchMissionState::None;

    /// <summary>Stage progress 0..1 for current stage.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float stageProgress01 = 0.f;

    /// <summary>Random seed.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 seed = 0;

    /// <summary>Internal stage duration (seconds).</summary>
    float stageDuration = 1.f;

    /// <summary>Internal stage time (seconds).</summary>
    float stageTime = 0.f;
};

#pragma endregion STRUCTS
