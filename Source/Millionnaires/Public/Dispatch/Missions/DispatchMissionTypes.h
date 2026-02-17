/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionTypes" - Header
 * Notes: Shared enums/structs for Dispatch mission system (centralized types).
 */
#pragma once

#include "CoreMinimal.h"
#include "DispatchMissionTypes.generated.h"

class UDispatchMissionDefinition;
class ADispatchMissionSiteActor;
class APawn;
class UTexture2D;
class UCharacterDefinition;

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

/// <summary>Difficulty metric displayed in the mission details UI.</summary>
UENUM(BlueprintType)
enum class EDispatchMissionDifficultyMetric : uint8
{
    MonsterChance       UMETA(DisplayName="Monster Chance"),
    LootChance          UMETA(DisplayName="Loot Chance"),
    ComplicationChance  UMETA(DisplayName="Complication Chance"),
};


/// <summary>Skills used to compute mission success chance. Extend freely.</summary>
UENUM(BlueprintType)
enum class EDispatchMissionSkill : uint8
{
    Combat       UMETA(DisplayName="Combat"),
    Engineering  UMETA(DisplayName="Engineering"),
    Medical      UMETA(DisplayName="Medical"),
    Security     UMETA(DisplayName="Security"),
    Science      UMETA(DisplayName="Science"),
    Diplomacy    UMETA(DisplayName="Diplomacy"),
    Logistics    UMETA(DisplayName="Logistics"),
};

#pragma endregion ENUMS



/// <summary>Success chance model used by mission definitions.</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionSuccessModel
{
    GENERATED_BODY()

    /// <summary>Minimum number of agents required to accept the mission.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="1", ClampMax="8"))
    int32 minAgents = 1;

    /// <summary>Maximum number of agents allowed for this mission.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="1", ClampMax="8"))
    int32 maxAgents = 3;

    /// <summary>Base success chance before bonuses/penalties (0..1).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="1.0"))
    float baseSuccessChance01 = 0.55f;

    /// <summary>
    /// Skill weights used to compute a weighted average skill score on 10.
    /// If empty, Combat=1 is assumed.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    TMap<EDispatchMissionSkill, float> skillWeights;

    /// <summary>How much 1 point of weighted skill score (on 10) adds to success chance.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="0.2"))
    float perSkillPoint10ToChance01 = 0.02f;

    /// <summary>Which difficulty metric applies a penalty to success chance.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    EDispatchMissionDifficultyMetric difficultyPenaltyMetric = EDispatchMissionDifficultyMetric::ComplicationChance;

    /// <summary>How much 1 point of difficulty (on 10) subtracts from success chance.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="0.2"))
    float difficultyPenaltyPerPoint10 = 0.02f;

    /// <summary>How much agent location affinity modifies success chance: (Affinity-1) * weight.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="1.0"))
    float affinityBonusWeight = 0.15f;

    /// <summary>Multiplier applied to equipment bonus (placeholder for inventory system).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="2.0"))
    float equipmentBonusWeight = 1.0f;

    /// <summary>Clamp min success chance.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="1.0"))
    float clampMin01 = 0.05f;

    /// <summary>Clamp max success chance.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success", meta=(ClampMin="0.0", ClampMax="1.0"))
    float clampMax01 = 0.95f;
};

/// <summary>Computed breakdown (useful for UI debugging).</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionSuccessBreakdown
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float baseChance01 = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float skillBonus01 = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float affinityBonus01 = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float equipmentBonus01 = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float difficultyPenalty01 = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float finalChance01 = 0.f;
};

/// <summary>UI entry representing a selectable agent (character) for missions.</summary>
USTRUCT(BlueprintType)
struct FDispatchSelectableAgentEntry
{
    GENERATED_BODY()

    /// <summary>Pawn in the world representing the agent.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    TObjectPtr<APawn> agentPawn = nullptr;

    /// <summary>Character definition used for display/stats.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    TObjectPtr<UCharacterDefinition> characterDefinition = nullptr;

    /// <summary>Stable character id (from definition).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    FName characterId = NAME_None;

    /// <summary>Display name.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    FText displayName;

    /// <summary>Portrait for UI.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    TObjectPtr<UTexture2D> portrait = nullptr;

    /// <summary>Whether this agent is already assigned to another active mission.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    bool bIsBusy = false;

    /// <summary>Whether this agent is selected for the currently viewed mission.</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|Agents")
    bool bIsSelected = false;
};

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

/// <summary>UI configuration for a difficulty bar (icon/title/slot visuals).</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionDifficultyUIConfig
{
    GENERATED_BODY()

    /// <summary>Which difficulty metric this row represents.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    EDispatchMissionDifficultyMetric metric = EDispatchMissionDifficultyMetric::MonsterChance;

    /// <summary>Icon shown in the header line.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> icon = nullptr;

    /// <summary>Title shown next to the icon.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    FText title;

    /// <summary>How many slots to render for this metric (default 10).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty", meta=(ClampMin="1", ClampMax="20"))
    int32 maxSlots = 10;

    /// <summary>Empty slot background texture.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> slotBackground = nullptr;

    /// <summary>Filled slot texture (shown for i &lt;= current).</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> slotFill = nullptr;
};

/// <summary>Runtime difficulty row data computed from an offer/mission.</summary>
USTRUCT(BlueprintType)
struct FDispatchMissionDifficultyUIEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    EDispatchMissionDifficultyMetric metric = EDispatchMissionDifficultyMetric::MonsterChance;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> icon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    FText title;

    /// <summary>Current filled slots count (0..maxSlots).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    int32 current = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    int32 maxSlots = 10;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> slotBackground = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TObjectPtr<UTexture2D> slotFill = nullptr;
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

    /// <summary>Difficulty values carried over from the source offer (used by UI).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FDispatchMissionDifficulty difficulty;


    /// <summary>Computed success chance at accept time (0..1).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Success")
    float successChance01 = 0.0f;

    /// <summary>Assigned character definitions (used for UI & persistence).</summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TArray<TObjectPtr<UCharacterDefinition>> assignedCharacters;

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
