/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Header
 * Notes: Generates mission offers, handles time-limited acceptance, and simulates Dispatch missions (FPS later).
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

    /** Mission mode. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /** Mission location dropdown value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /** Actor representing the mission site (building/zone group). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<ADispatchMissionSiteActor> locationActor = nullptr;

    /** World location where this mission happens (usually locationActor->GetActorLocation()). */
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

    /** Offer time limit rolled for this offer (seconds). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float timeLimitSec = 60.f;

    /** Remaining time before expiry (seconds). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    float timeRemainingSec = 60.f;
};

/** Active mission in progress (after accepting an offer). */
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

    /** Mission mode. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionMode missionMode = EDispatchMissionMode::Dispatch;

    /** Mission location dropdown. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionLocation missionLocation = EDispatchMissionLocation::Any;

    /** Actor representing the mission site. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TObjectPtr<ADispatchMissionSiteActor> locationActor = nullptr;

    /** World location. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    FVector worldLocation = FVector::ZeroVector;

    /** Assigned agents (AI pawns). FPS missions should contain exactly 1 agent. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    TArray<TObjectPtr<APawn>> assignedAgents;

    /** Mission state. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dispatch|Missions")
    EDispatchMissionState state = EDispatchMissionState::None;

    /** Stage progress 0..1 for current stage. */
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
 * Component that spawns time-limited mission offers and simulates Dispatch missions.
 * Attach to your Dispatch PlayerController (or a central Dispatch manager actor).
 */
UCLASS(ClassGroup=(Dispatch), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UDispatchMissionManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
#pragma region LIFECYCLE

    UDispatchMissionManagerComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion LIFECYCLE

#pragma region API_RUN

    /** Resets day counter, clears offers/missions, and restarts spawning. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Run")
    void StartNewRun();

    /** Advances current day number (DAY 1..). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Run")
    void AdvanceDay();

    /** Returns current day. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Run")
    int32 GetCurrentDay() const { return currentDay; }

#pragma endregion API_RUN

#pragma region API_OFFERS

    /** Returns a copy of current offers. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions|Offers")
    TArray<FDispatchMissionOffer> GetOffers() const { return offers; }

    /** Finds an offer by id. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool TryGetOffer(const FGuid& OfferId, FDispatchMissionOffer& OutOffer) const;

    /**
     * Accepts an offer with the selected agents.
     * - Dispatch missions: 1..N agents
     * - FPS missions: exactly 1 agent (future)
     */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool AcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents);

    /** Declines an offer (removes it). */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Missions|Offers")
    bool DeclineOffer(const FGuid& OfferId);

#pragma endregion API_OFFERS

#pragma region API_MISSIONS

    /** Returns a copy of all active missions. */
    UFUNCTION(BlueprintPure, Category="Dispatch|Missions")
    TArray<FDispatchActiveMission> GetActiveMissions() const { return activeMissions; }

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

#pragma endregion EVENTS

#pragma region SETTINGS

    /** All available mission definitions. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Definitions")
    TArray<TObjectPtr<UDispatchMissionDefinition>> availableDefinitions;

    /** If true, offers are spawned automatically over time. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    bool bAutoSpawnOffers = true;

    /** Range of seconds between offer spawns. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    FVector2D offerIntervalRangeSec = FVector2D(12.f, 25.f);

    /** Default offer time limit range (seconds) used when definitions don't override it. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Offers")
    FVector2D defaultOfferTimeLimitRangeSec = FVector2D(45.f, 90.f);

    /** If true, offers spawn on Mission Sites (actors placed in level) using the missionLocation dropdown. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Missions|Locations")
    bool bUseMissionSites = true;

#pragma endregion SETTINGS

protected:
#pragma region INTERNAL

    void ResetState();
    void ScheduleNextOffer(FRandomStream& Rng);
    void GenerateOffer();
    void UpdateOfferTimers(float DeltaTime);
    void UpdateMissionSimulation(float DeltaTime);

    /** Picks a site actor matching the requested location (or any). */
    ADispatchMissionSiteActor* PickMissionSite(EDispatchMissionLocation Location, FRandomStream& Rng) const;

    FVector GetFallbackWorldLocation(FRandomStream& Rng) const;
    FDispatchMissionDifficulty RollDifficulty(UDispatchMissionDefinition* Def, FRandomStream& Rng) const;

    void SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState);

#pragma endregion INTERNAL

private:
#pragma region STATE

    int32 currentDay = 1;

    /** Offer spawn timer. */
    float timeUntilNextOffer = 3.f;

    /** Offers currently available. */
    UPROPERTY(VisibleAnywhere, Category="Dispatch|Missions")
    TArray<FDispatchMissionOffer> offers;

    /** Active missions in progress. */
    UPROPERTY(VisibleAnywhere, Category="Dispatch|Missions")
    TArray<FDispatchActiveMission> activeMissions;

    /** Seed for deterministic run behavior if needed. */
    UPROPERTY(EditAnywhere, Category="Dispatch|Missions")
    int32 runSeed = 1337;

#pragma endregion STATE
};
