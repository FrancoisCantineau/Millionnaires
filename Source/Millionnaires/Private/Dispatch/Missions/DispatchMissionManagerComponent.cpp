/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Source
 * Notes: Runtime mission offers and Dispatch mission simulation.
 */
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

#pragma region LIFECYCLE

UDispatchMissionManagerComponent::UDispatchMissionManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDispatchMissionManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    StartNewRun();

    if (bAutoDiscoverAgents)
    {
        DiscoverAgents();
    }

    ScheduleNextOffer();
}

void UDispatchMissionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(offerTimerHandle);
    }

    Super::EndPlay(EndPlayReason);
}

void UDispatchMissionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateMissionSimulation(DeltaTime);
}

#pragma endregion LIFECYCLE

#pragma region API_DAY

void UDispatchMissionManagerComponent::StartNewRun()
{
    currentDay = 1;
    offers.Reset();
    activeMissions.Reset();

    OnDayChanged.Broadcast(currentDay);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] StartNewRun -> Day %d"), currentDay);
}

void UDispatchMissionManagerComponent::AdvanceDay()
{
    currentDay = FMath::Max(1, currentDay + 1);
    OnDayChanged.Broadcast(currentDay);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] AdvanceDay -> Day %d"), currentDay);
}

#pragma endregion API_DAY

#pragma region API_OFFERS

bool UDispatchMissionManagerComponent::AcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents)
{
    const int32 OfferIndex = FindOfferIndex(OfferId);
    if (OfferIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer failed: offer not found."));
        return false;
    }

    const FDispatchMissionOffer Offer = offers[OfferIndex];
    if (!Offer.definition)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer failed: definition is null."));
        return false;
    }

    // Validate agent selection according to mission type.
    const int32 AgentCount = SelectedAgents.Num();

    if (Offer.definition->missionType == EDispatchMissionType::Dispatch)
    {
        if (AgentCount < Offer.definition->minAgents || AgentCount > Offer.definition->maxAgents)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer failed: invalid agent count (%d)."), AgentCount);
            return false;
        }
    }
    else
    {
        // FPS mission: enforce single agent now (future takeover).
        if (Offer.definition->bFpsSingleAgentOnly && AgentCount != 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer failed: FPS missions require exactly one agent."));
            return false;
        }
    }

    // Create mission instance.
    FDispatchActiveMission Mission;
    Mission.missionId = FGuid::NewGuid();
    Mission.sourceOfferId = Offer.offerId;
    Mission.definition = Offer.definition;
    Mission.missionType = Offer.definition->missionType;
    Mission.state = EDispatchMissionState::Accepted;
    Mission.worldLocation = Offer.worldLocation;
    Mission.difficulty = Offer.difficulty;
    Mission.seed = Offer.seed;

    for (APawn* P : SelectedAgents)
    {
        if (P)
        {
            Mission.assignedAgents.Add(P);
        }
    }

    activeMissions.Add(Mission);

    // Remove offer.
    offers.RemoveAt(OfferIndex);
    OnOfferRemoved.Broadcast(OfferId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Mission accepted | MissionId=%s | Type=%d | Agents=%d"),
        *Mission.missionId.ToString(), (int32)Mission.missionType, Mission.assignedAgents.Num()
    );

    // Start simulation (Dispatch missions only). FPS missions will later transition to "player takeover".
    if (Mission.missionType == EDispatchMissionType::Dispatch)
    {
        StartMissionSimulation(activeMissions.Last());
    }
    else
    {
        // For now: mark as Traveling so UI can visualize it, but gameplay takeover is not implemented yet.
        StartMissionSimulation(activeMissions.Last());
    }

    return true;
}

bool UDispatchMissionManagerComponent::DeclineOffer(const FGuid& OfferId)
{
    const int32 OfferIndex = FindOfferIndex(OfferId);
    if (OfferIndex == INDEX_NONE)
    {
        return false;
    }

    offers.RemoveAt(OfferIndex);
    OnOfferRemoved.Broadcast(OfferId);
    return true;
}

#pragma endregion API_OFFERS

#pragma region INTERNAL

void UDispatchMissionManagerComponent::ScheduleNextOffer()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (maxOffers <= 0)
    {
        return;
    }

    const float MinT = FMath::Max(0.1f, offerIntervalRangeSec.X);
    const float MaxT = FMath::Max(MinT, offerIntervalRangeSec.Y);

    const float Delay = FMath::FRandRange(MinT, MaxT);

    World->GetTimerManager().SetTimer(
        offerTimerHandle,
        this,
        &UDispatchMissionManagerComponent::GenerateOffer,
        Delay,
        false
    );
}

void UDispatchMissionManagerComponent::GenerateOffer()
{
    if (maxOffers > 0 && offers.Num() >= maxOffers)
    {
        ScheduleNextOffer();
        return;
    }

    if (missionDefinitions.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] No missionDefinitions set."));
        ScheduleNextOffer();
        return;
    }

    // Pick a random definition.
    const int32 DefIndex = FMath::RandRange(0, missionDefinitions.Num() - 1);
    UDispatchMissionDefinition* Def = missionDefinitions[DefIndex];
    if (!Def)
    {
        ScheduleNextOffer();
        return;
    }

    FDispatchMissionOffer Offer;
    Offer.offerId = FGuid::NewGuid();
    Offer.definition = Def;
    Offer.dayCreated = currentDay;
    Offer.seed = FMath::Rand();
    FRandomStream Rng(Offer.seed);

    Offer.worldLocation = PickMissionLocation(Rng);
    Offer.difficulty = RollDifficulty(Def, Rng);

    offers.Add(Offer);
    OnOfferAdded.Broadcast(Offer.offerId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer generated | OfferId=%s | Type=%d | Loc=%s"),
        *Offer.offerId.ToString(),
        (int32)Def->missionType,
        *Offer.worldLocation.ToString()
    );

    ScheduleNextOffer();
}

FDispatchMissionDifficulty UDispatchMissionManagerComponent::RollDifficulty(const UDispatchMissionDefinition* Def, FRandomStream& Rng) const
{
    FDispatchMissionDifficulty D;
    if (!Def)
    {
        return D;
    }

    D.monsterChance10 = Def->difficultyRanges.monsterChance10.RandomInRange(Rng);
    D.lootChance10 = Def->difficultyRanges.lootChance10.RandomInRange(Rng);
    D.complicationChance10 = Def->difficultyRanges.complicationChance10.RandomInRange(Rng);
    return D;
}

FVector UDispatchMissionManagerComponent::PickMissionLocation(FRandomStream& Rng) const
{
    if (!bUseTaggedMissionLocations)
    {
        // Fallback: random near base.
        const FVector Base = GetBaseLocation();
        const float Radius = 2500.f;
        const FVector Dir3 = Rng.VRand();
        FVector2D Rand2(Dir3.X, Dir3.Y);
        Rand2 = Rand2.GetSafeNormal();
        return Base + FVector(Rand2.X, Rand2.Y, 0.f) * Radius;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return FVector::ZeroVector;
    }

    TArray<AActor*> Candidates;
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        if (It->ActorHasTag(missionLocationTag))
        {
            Candidates.Add(*It);
        }
    }

    if (Candidates.Num() == 0)
    {
        return GetBaseLocation();
    }

    const int32 Index = Rng.RandRange(0, Candidates.Num() - 1);
    return Candidates[Index]->GetActorLocation();
}

FVector UDispatchMissionManagerComponent::GetBaseLocation() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return FVector::ZeroVector;
    }

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        if (It->ActorHasTag(baseAnchorTag))
        {
            return It->GetActorLocation();
        }
    }

    // Fallback to owner location.
    if (const AActor* OwnerActor = GetOwner())
    {
        return OwnerActor->GetActorLocation();
    }

    return FVector::ZeroVector;
}

void UDispatchMissionManagerComponent::DiscoverAgents()
{
    availableAgents.Reset();

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<APawn> It(World); It; ++It)
    {
        if (It->ActorHasTag(agentTag))
        {
            availableAgents.Add(*It);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Discovered %d agents (Tag=%s)."), availableAgents.Num(), *agentTag.ToString());
}

void UDispatchMissionManagerComponent::StartMissionSimulation(FDispatchActiveMission& Mission)
{
    const FVector Base = GetBaseLocation();
    const float Dist = FVector::Dist(Base, Mission.worldLocation);

    float Speed = 400.f;
    if (Mission.definition)
    {
        Speed = FMath::Max(1.f, Mission.definition->travelSpeedCmPerSec);
    }

    const float TravelDuration = FMath::Max(0.1f, Dist / Speed);
    SetMissionState(Mission, EDispatchMissionState::Traveling, TravelDuration);
}

void UDispatchMissionManagerComponent::SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState, float StageDurationSec)
{
    Mission.state = NewState;
    Mission.stageDuration = FMath::Max(0.1f, StageDurationSec);
    Mission.stageTime = 0.f;
    Mission.stageProgress01 = 0.f;

    OnMissionStateChanged.Broadcast(Mission.missionId, NewState);
}

void UDispatchMissionManagerComponent::UpdateMissionSimulation(float DeltaTime)
{
    for (int32 i = activeMissions.Num() - 1; i >= 0; --i)
    {
        FDispatchActiveMission& M = activeMissions[i];

        if (M.state == EDispatchMissionState::Completed || M.state == EDispatchMissionState::Failed || M.state == EDispatchMissionState::Aborted)
        {
            continue;
        }

        M.stageTime += DeltaTime;
        M.stageProgress01 = FMath::Clamp(M.stageTime / FMath::Max(0.1f, M.stageDuration), 0.f, 1.f);

        OnMissionProgress.Broadcast(M.missionId, M.stageProgress01);

        if (M.stageProgress01 < 1.f)
        {
            continue;
        }

        // Stage finished -> next state
        if (M.state == EDispatchMissionState::Traveling)
        {
            // Resolve stage (later: combat/rolls/complications).
            SetMissionState(M, EDispatchMissionState::Resolving, 3.f);
        }
        else if (M.state == EDispatchMissionState::Resolving)
        {
            // Return stage
            const FVector Base = GetBaseLocation();
            const float Dist = FVector::Dist(Base, M.worldLocation);

            float Speed = 450.f;
            if (M.definition)
            {
                Speed = FMath::Max(1.f, M.definition->returnSpeedCmPerSec);
            }

            const float ReturnDuration = FMath::Max(0.1f, Dist / Speed);
            SetMissionState(M, EDispatchMissionState::Returning, ReturnDuration);
        }
        else if (M.state == EDispatchMissionState::Returning)
        {
            SetMissionState(M, EDispatchMissionState::Completed, 0.1f);
            UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Mission completed | MissionId=%s"), *M.missionId.ToString());
        }
    }
}

int32 UDispatchMissionManagerComponent::FindOfferIndex(const FGuid& OfferId) const
{
    for (int32 i = 0; i < offers.Num(); ++i)
    {
        if (offers[i].offerId == OfferId)
        {
            return i;
        }
    }
    return INDEX_NONE;
}

#pragma endregion INTERNAL


bool UDispatchMissionManagerComponent::TryGetOffer(const FGuid& OfferId, FDispatchMissionOffer& OutOffer) const
{
    const int32 Index = FindOfferIndex(OfferId);
    if (Index == INDEX_NONE)
    {
        return false;
    }

    OutOffer = offers[Index];
    return true;
}
