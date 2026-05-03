/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Header
 * Notes: Generates mission offers, handles time-limited acceptance, and simulates Dispatch missions.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionManagerComponent.generated.h"

class UDispatchMissionDefinition;
class ADispatchMissionSiteActor;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferAdded, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferRemoved, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferExpired, const FGuid&, OfferId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferAccepted, const FGuid&, OfferId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchMissionStateChanged, const FGuid&, MissionId, EDispatchMissionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchMissionProgress, const FGuid&, MissionId, float, Progress01);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchMissionTimePausedChanged, bool, bIsPaused);

#pragma endregion DELEGATES

/**
 * Component that spawns time-limited mission offers and simulates Dispatch missions.
 * Attach to your Dispatch PlayerController (or a central Dispatch manager actor).
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    /// <summary>Constructor.</summary>
    UDispatchMissionManagerComponent();

    /// <summary>Called when the game starts.</summary>
    virtual void BeginPlay() override;

    /// <summary>Tick: updates offer timers, offer spawning, and mission simulation.</summary>
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region API_RUN

    /// <summary>Resets day counter, clears offers/missions, and restarts spawning.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Run")
    void StartNewRun();

    /// <summary>Advances current day number (DAY 1..).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Run")
    void AdvanceDay();

    /// <summary>Returns current day.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Run")
    int32 GetCurrentDay() const { return currentDay; }

#pragma endregion API_RUN

#pragma region API_TIME

    /**
     * Pauses/unpauses mission time.
     * While paused:
     * - offer timers don't tick (no expiry)
     * - no new offers spawn
     * - active mission simulation doesn't progress
     */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Time")
    void SetMissionTimePaused(bool bPaused);

    /// <summary>Returns whether mission time is paused.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Time")
    bool IsMissionTimePaused() const { return bMissionTimePaused; }

#pragma endregion API_TIME

#pragma region API_OFFERS

    /// <summary>Returns a copy of current offers.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Offers")
    TArray<FDispatchMissionOffer> GetOffers() const { return offers; }

    /// <summary>Finds an offer by id.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool TryGetOffer(const FGuid& OfferId, FDispatchMissionOffer& OutOffer) const;

    /**
     * Accepts an offer with the selected agents.
     * - Dispatch missions: 1..N agents
     * - FPS missions: exactly 1 agent (future)
     */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool AcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents);

    /// <summary>Declines an offer (removes it).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool DeclineOffer(const FGuid& OfferId);

#pragma endregion API_OFFERS

#pragma region API_SUCCESS

    /// <summary>Returns whether a pawn is already assigned to any active mission.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Agents")
    bool IsAgentBusy(APawn* Agent) const;

    /// <summary>Computes success chance for an offer given selected agents (0..1) and outputs a breakdown.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Success")
    bool ComputeOfferSuccessChance(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents, float& OutChance01, FDispatchMissionSuccessBreakdown& OutBreakdown) const;

#pragma endregion API_SUCCESS

 #pragma region API_MISSIONS

    /// <summary>Returns a copy of all active missions.</summary>
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions")
    TArray<FDispatchActiveMission> GetActiveMissions() const { return activeMissions; }

    /// <summary>Finds an active mission by id.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions")
    bool TryGetActiveMission(const FGuid& MissionId, FDispatchActiveMission& OutMission) const;

    /// <summary>Finds an active mission by source offer id (useful right after accept).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions")
    bool TryGetActiveMissionFromOffer(const FGuid& SourceOfferId, FDispatchActiveMission& OutMission) const;

#pragma endregion API_MISSIONS

#pragma region EVENTS

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferAdded OnOfferAdded;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferRemoved OnOfferRemoved;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferExpired OnOfferExpired;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchOfferAccepted OnOfferAccepted;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchMissionStateChanged OnMissionStateChanged;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchMissionProgress OnMissionProgress;

    /// <summary>Fired when time pause changes.</summary>
    UPROPERTY(BlueprintAssignable, Category="Dispatch|Missions|Events")
    FDispatchMissionTimePausedChanged OnMissionTimePausedChanged;

#pragma endregion EVENTS

#pragma region SETTINGS

    /// <summary>All available mission definitions.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Definitions")
    TArray<TObjectPtr<UDispatchMissionDefinition>> availableDefinitions;

    /// <summary>If true, offers are spawned automatically over time.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    bool bAutoSpawnOffers = true;

    /// <summary>Range of seconds between offer spawns.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    FVector2D offerIntervalRangeSec = FVector2D(12.f, 25.f);

    /// <summary>Default offer time limit range (seconds) used when definitions don't override it.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    FVector2D defaultOfferTimeLimitRangeSec = FVector2D(45.f, 90.f);

    /// <summary>If true, offers spawn on Mission Sites (actors placed in level) using the missionLocation dropdown.</summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Locations")
    bool bUseMissionSites = true;

#pragma endregion SETTINGS

protected:
#pragma region INTERNAL

    /// <summary>Clears internal runtime state.</summary>
    void ResetState();

    /// <summary>Computes the next offer spawn delay.</summary>
    void ScheduleNextOffer(FRandomStream& Rng);

    /// <summary>Generates a mission offer from a random definition.</summary>
    void GenerateOffer();

    /// <summary>Decrements remaining time of active offers.</summary>
    void UpdateOfferTimers(float DeltaTime);

    /// <summary>Updates dispatch-mode mission simulation.</summary>
    void UpdateMissionSimulation(float DeltaTime);

    /// <summary>Picks a site actor matching the requested location (or any).</summary>
    ADispatchMissionSiteActor* PickMissionSite(EDispatchMissionLocation Location, FRandomStream& Rng) const;

    /// <summary>Fallback world position when no sites are available.</summary>
    FVector GetFallbackWorldLocation(FRandomStream& Rng) const;

    /// <summary>Rolls difficulty values from definition ranges.</summary>
    FDispatchMissionDifficulty RollDifficulty(UDispatchMissionDefinition* Def, FRandomStream& Rng) const;

    /// <summary>Changes mission state and rolls next stage duration.</summary>
    void SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState);

#pragma endregion INTERNAL

private:
#pragma region STATE

    int32 currentDay = 1;

    /// <summary>Offer spawn timer.</summary>
    float timeUntilNextOffer = 3.f;

    /// <summary>Offers currently available.</summary>
    UPROPERTY(VisibleAnywhere, Category="Dispatch|Missions")
    TArray<FDispatchMissionOffer> offers;

    /// <summary>Active missions in progress.</summary>
    UPROPERTY(VisibleAnywhere, Category="Dispatch|Missions")
    TArray<FDispatchActiveMission> activeMissions;

    /// <summary>Seed for deterministic run behavior if needed.</summary>
    UPROPERTY(EditAnywhere, Category="Dispatch|Missions")
    int32 runSeed = 1337;

    /// <summary>If true, time is paused for offers and missions.</summary>
    UPROPERTY(VisibleAnywhere, Category="Dispatch|Missions")
    bool bMissionTimePaused = false;

    /// <summary>Internal monotonic counter used to vary seeds across offers.</summary>
    int32 offerCounter = 0;

    /// <summary>
    /// Stores the actual rolled travel duration per mission so Returning can reuse the same duration.
    /// Cleared when the mission ends.
    /// </summary>
    TMap<FGuid, float> travelDurationByMissionSec;

#pragma endregion STATE
};
