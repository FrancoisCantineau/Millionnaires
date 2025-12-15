/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Header
 * Notes: Spawns mission offers randomly and simulates Dispatch missions travel/return.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionManagerComponent.generated.h"

class UDispatchMissionDefinition;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchDayChanged, int32, NewDay);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferAdded, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferRemoved, const FGuid&, OfferId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchMissionStateChanged, const FGuid&, MissionId, EDispatchMissionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchMissionProgress, const FGuid&, MissionId, float, Progress01);

#pragma endregion DELEGATES

#pragma region STRUCTS

/** A mission offer generated at runtime from a definition. */
USTRUCT(BlueprintType)
struct FDispatchMissionOffer
{
    GENERATED_BODY()

    /** Unique offer id. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid offerId;

    /** Definition asset used to generate this offer. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<UDispatchMissionDefinition> definition = nullptr;

    /** World location where this mission happens. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FVector worldLocation = FVector::ZeroVector;

    /** Difficulty rolled from definition. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FDispatchMissionDifficulty difficulty;

    /** Day when this offer was created. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 dayCreated = 1;

    /** Random seed used for loot & outcomes (deterministic if reused). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 seed = 0;
};

/** A mission currently running (accepted). */
USTRUCT(BlueprintType)
struct FDispatchActiveMission
{
    GENERATED_BODY()

    /** Mission id. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid missionId;

    /** Offer id this mission came from. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FGuid sourceOfferId;

    /** Definition. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<UDispatchMissionDefinition> definition = nullptr;

    /** Mission type. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionType missionType = EDispatchMissionType::Dispatch;

    /** State. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionState state = EDispatchMissionState::Accepted;

    /** Mission world location. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FVector worldLocation = FVector::ZeroVector;

    /** Difficulty. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FDispatchMissionDifficulty difficulty;

    /** Assigned agents (AI pawns). FPS missions should contain exactly 1 agent. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TArray<TObjectPtr<APawn>> assignedAgents;

    /** Travel progress 0..1 for current stage. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float stageProgress01 = 0.f;

    /** Random seed. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    int32 seed = 0;

    /** Internal stage duration (seconds). */
    float stageDuration = 1.f;

    /** Internal stage time (seconds). */
    float stageTime = 0.f;
};

#pragma endregion STRUCTS

/**
 * Mission manager component:
 * - starts at Day 1
 * - spawns mission offers at random intervals
 * - accepts offers and simulates Dispatch missions (travel -> resolve -> return)
 *
 * Future FPS missions are supported by validating "single agent" and by emitting state changes,
 * but the actual player takeover is left for later.
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    UDispatchMissionManagerComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region API_DAY

    /** Resets the system to Day 1 and clears offers/missions. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Day")
    void StartNewRun();

    /** Advances the day by one (optional for later). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Day")
    void AdvanceDay();

    /** Returns the current day. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Day")
    int32 GetCurrentDay() const { return currentDay; }

#pragma endregion API_DAY

#pragma region API_OFFERS

    /** Returns a copy of all active offers. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Offers")
    TArray<FDispatchMissionOffer> GetOffers() const { return offers; }

    /** Tries to find an offer by id. Returns true if found. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Offers")
    bool TryGetOffer(const FGuid& OfferId, FDispatchMissionOffer& OutOffer) const;

    /** Accepts an offer and starts a mission (Dispatch or FPS). Returns true on success. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool AcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents);

    /** Declines (removes) an offer. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool DeclineOffer(const FGuid& OfferId);

#pragma endregion API_OFFERS

#pragma region API_MISSIONS

    /** Returns a copy of active missions. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Missions")
    TArray<FDispatchActiveMission> GetActiveMissions() const { return activeMissions; }

#pragma endregion API_MISSIONS

#pragma region EVENTS

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchDayChanged OnDayChanged;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferAdded OnOfferAdded;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferRemoved OnOfferRemoved;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchMissionStateChanged OnMissionStateChanged;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchMissionProgress OnMissionProgress;

#pragma endregion EVENTS

#pragma region CONFIG

    /** Mission definitions available for offer generation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    TArray<TObjectPtr<UDispatchMissionDefinition>> missionDefinitions;

    /** If true, automatically discovers agents by tag at BeginPlay. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    bool bAutoDiscoverAgents = true;

    /** Pawn tag used to identify base agents. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    FName agentTag = FName("DispatchAgent");

    /** If true, picks a random actor tagged as missionLocationTag for offer locations. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    bool bUseTaggedMissionLocations = true;

    /** Actor tag used as potential mission locations. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    FName missionLocationTag = FName("DispatchMissionLocation");

    /** Actor tag used as base location anchor (for distance simulation). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config")
    FName baseAnchorTag = FName("DispatchBase");

    /** Offer spawn interval range (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config", meta=(ClampMin="0.1"))
    FVector2D offerIntervalRangeSec = FVector2D(10.f, 25.f);

    /** Maximum offers alive at the same time. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Config", meta=(ClampMin="0"))
    int32 maxOffers = 3;

#pragma endregion CONFIG

protected:
#pragma region INTERNAL

    /** Schedules the next offer spawn timer. */
    void ScheduleNextOffer();

    /** Generates a new offer if possible. */
    void GenerateOffer();

    /** Rolls difficulty from a definition. */
    FDispatchMissionDifficulty RollDifficulty(const UDispatchMissionDefinition* Def, FRandomStream& Rng) const;

    /** Picks a random mission location in the world. */
    FVector PickMissionLocation(FRandomStream& Rng) const;

    /** Finds base anchor location. */
    FVector GetBaseLocation() const;

    /** Discover agents in the world by tag. */
    void DiscoverAgents();

    /** Starts simulation for a mission (Traveling stage). */
    void StartMissionSimulation(FDispatchActiveMission& Mission);

    /** Moves mission state and resets stage timers. */
    void SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState, float StageDurationSec);

    /** Updates mission simulation each tick. */
    void UpdateMissionSimulation(float DeltaTime);

    /** Finds offer index by id. */
    int32 FindOfferIndex(const FGuid& OfferId) const;

#pragma endregion INTERNAL

private:
#pragma region STATE

    /** Current day. */
    int32 currentDay = 1;

    /** Mission offers. */
    UPROPERTY(Transient)
    TArray<FDispatchMissionOffer> offers;

    /** Active missions. */
    UPROPERTY(Transient)
    TArray<FDispatchActiveMission> activeMissions;

    /** Cached agents that can be dispatched. */
    UPROPERTY(Transient)
    TArray<TObjectPtr<APawn>> availableAgents;

    /** Offer timer handle. */
    FTimerHandle offerTimerHandle;

#pragma endregion STATE
};
