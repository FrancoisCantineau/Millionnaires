/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Source
 * Notes: Generates mission offers, handles time-limited acceptance, and simulates Dispatch missions (FPS later).
 */
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"
#include "Dispatch/Map/DispatchMissionSiteActor.h"

#include "Engine/World.h"
#include "EngineUtils.h"

#pragma region LIFECYCLE

UDispatchMissionManagerComponent::UDispatchMissionManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDispatchMissionManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    StartNewRun();
}

void UDispatchMissionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Offers update
    UpdateOfferTimers(DeltaTime);

    // Offer generation
    if (bAutoSpawnOffers && availableDefinitions.Num() > 0)
    {
        timeUntilNextOffer -= DeltaTime;
        if (timeUntilNextOffer <= 0.f)
        {
            GenerateOffer();

            FRandomStream Rng(runSeed + currentDay * 1000 + offers.Num() * 17);
            ScheduleNextOffer(Rng);
        }
    }

    // Missions update
    UpdateMissionSimulation(DeltaTime);
}

#pragma endregion LIFECYCLE

#pragma region API_RUN

void UDispatchMissionManagerComponent::StartNewRun()
{
    ResetState();

    FRandomStream Rng(runSeed);
    ScheduleNextOffer(Rng);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] New run started | Day=%d | Seed=%d"), currentDay, runSeed);
}

void UDispatchMissionManagerComponent::AdvanceDay()
{
    currentDay = FMath::Max(1, currentDay + 1);
    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] AdvanceDay | Day=%d"), currentDay);
}

#pragma endregion API_RUN

#pragma region API_OFFERS

bool UDispatchMissionManagerComponent::TryGetOffer(const FGuid& OfferId, FDispatchMissionOffer& OutOffer) const
{
    for (const FDispatchMissionOffer& O : offers)
    {
        if (O.offerId == OfferId)
        {
            OutOffer = O;
            return true;
        }
    }

    return false;
}

bool UDispatchMissionManagerComponent::AcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents)
{
    const int32 OfferIndex = offers.IndexOfByPredicate([&](const FDispatchMissionOffer& O){ return O.offerId == OfferId; });
    if (OfferIndex == INDEX_NONE)
    {
        return false;
    }

    const FDispatchMissionOffer Offer = offers[OfferIndex];

    // Validate agent selection
    if (Offer.missionMode == EDispatchMissionMode::FPS)
    {
        if (SelectedAgents.Num() != 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (FPS requires exactly 1 agent)."));
            return false;
        }
    }
    else
    {
        if (SelectedAgents.Num() <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (Dispatch requires at least 1 agent)."));
            return false;
        }
    }

    // Create mission
    FDispatchActiveMission Mission;
    Mission.missionId = FGuid::NewGuid();
    Mission.sourceOfferId = Offer.offerId;
    Mission.definition = Offer.definition;
    Mission.missionMode = Offer.missionMode;
    Mission.missionLocation = Offer.missionLocation;
    Mission.locationActor = Offer.locationActor;
    Mission.worldLocation = Offer.worldLocation;
    Mission.seed = Offer.seed;

    for (APawn* P : SelectedAgents)
    {
        Mission.assignedAgents.Add(P);
    }

    // Init first stage
    Mission.state = EDispatchMissionState::Accepted;
    SetMissionState(Mission, EDispatchMissionState::Traveling);

    activeMissions.Add(Mission);

    OnOfferAccepted.Broadcast(OfferId);

    // Remove offer
    offers.RemoveAt(OfferIndex);
    OnOfferRemoved.Broadcast(OfferId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer accepted | OfferId=%s | MissionId=%s | Agents=%d"),
        *OfferId.ToString(), *Mission.missionId.ToString(), SelectedAgents.Num());

    return true;
}

bool UDispatchMissionManagerComponent::DeclineOffer(const FGuid& OfferId)
{
    const int32 OfferIndex = offers.IndexOfByPredicate([&](const FDispatchMissionOffer& O){ return O.offerId == OfferId; });
    if (OfferIndex == INDEX_NONE)
    {
        return false;
    }

    offers.RemoveAt(OfferIndex);
    OnOfferRemoved.Broadcast(OfferId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer declined | OfferId=%s"), *OfferId.ToString());
    return true;
}

#pragma endregion API_OFFERS

#pragma region INTERNAL

void UDispatchMissionManagerComponent::ResetState()
{
    currentDay = 1;
    offers.Reset();
    activeMissions.Reset();
    timeUntilNextOffer = 3.f;
}

void UDispatchMissionManagerComponent::ScheduleNextOffer(FRandomStream& Rng)
{
    const float MinT = FMath::Max(0.25f, offerIntervalRangeSec.X);
    const float MaxT = FMath::Max(MinT, offerIntervalRangeSec.Y);
    timeUntilNextOffer = Rng.FRandRange(MinT, MaxT);
}

void UDispatchMissionManagerComponent::GenerateOffer()
{
    if (availableDefinitions.Num() == 0)
    {
        return;
    }

    FRandomStream Rng(runSeed + currentDay * 1000 + offers.Num() * 17);

    // Pick definition
    const int32 DefIndex = Rng.RandRange(0, availableDefinitions.Num() - 1);
    UDispatchMissionDefinition* Def = availableDefinitions[DefIndex];
    if (!Def)
    {
        return;
    }

    FDispatchMissionOffer Offer;
    Offer.offerId = FGuid::NewGuid();
    Offer.definition = Def;
    Offer.missionMode = Def->missionMode;
    Offer.missionLocation = Def->missionLocation;
    Offer.dayCreated = currentDay;
    Offer.seed = Rng.RandRange(1, 2147483646);

    // Pick site (building/zone) from dropdown
    ADispatchMissionSiteActor* Site = PickMissionSite(Offer.missionLocation, Rng);
    Offer.locationActor = Site;
    Offer.worldLocation = Site ? Site->GetActorLocation() : GetFallbackWorldLocation(Rng);
     UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer generated | OfferId=%s | Def=%s | Location=%d | Site=%s"),
         *Offer.offerId.ToString(), *GetNameSafe(Def), (int32)Offer.missionLocation, *GetNameSafe(Site));
    if (!Site)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] No MissionSiteActor found for Location=%d. Place sites in the level and set their MissionLocation."),
             (int32)Offer.missionLocation);
    }
    
    // Roll difficulty
    Offer.difficulty = RollDifficulty(Def, Rng);

    // Time limit
    float LimitMin = defaultOfferTimeLimitRangeSec.X;
    float LimitMax = defaultOfferTimeLimitRangeSec.Y;
    if (Def)
    {
        LimitMin = FMath::Max(1.f, Def->offerTimeLimitMinSec);
        LimitMax = FMath::Max(LimitMin, Def->offerTimeLimitMaxSec);
    }

    Offer.timeLimitSec = Rng.FRandRange(LimitMin, LimitMax);
    Offer.timeRemainingSec = Offer.timeLimitSec;

    offers.Add(Offer);

    OnOfferAdded.Broadcast(Offer.offerId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer generated | OfferId=%s | Mode=%d | Location=%d | Time=%.1fs"),
        *Offer.offerId.ToString(), (int32)Offer.missionMode, (int32)Offer.missionLocation, Offer.timeLimitSec);
}

void UDispatchMissionManagerComponent::UpdateOfferTimers(float DeltaTime)
{
    if (offers.Num() == 0)
    {
        return;
    }

    for (int32 i = offers.Num() - 1; i >= 0; --i)
    {
        FDispatchMissionOffer& O = offers[i];
        O.timeLimitSec = FMath::Max(1.f, O.timeLimitSec);
        O.timeRemainingSec = FMath::Clamp(O.timeRemainingSec - DeltaTime, 0.f, O.timeLimitSec);

        if (O.timeRemainingSec <= 0.f)
        {
            const FGuid ExpiredId = O.offerId;
            offers.RemoveAt(i);

            OnOfferExpired.Broadcast(ExpiredId);
            OnOfferRemoved.Broadcast(ExpiredId);

            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] Offer expired | OfferId=%s"), *ExpiredId.ToString());
        }
    }
}

void UDispatchMissionManagerComponent::UpdateMissionSimulation(float DeltaTime)
{
    for (int32 i = activeMissions.Num() - 1; i >= 0; --i)
    {
        FDispatchActiveMission& M = activeMissions[i];

        // Only simulate dispatch mode for now (FPS missions will be player-driven later).
        if (M.missionMode == EDispatchMissionMode::FPS)
        {
            continue;
        }

        M.stageTime += DeltaTime;
        M.stageDuration = FMath::Max(0.1f, M.stageDuration);
        M.stageProgress01 = FMath::Clamp(M.stageTime / M.stageDuration, 0.f, 1.f);

        OnMissionProgress.Broadcast(M.missionId, M.stageProgress01);

        if (M.stageProgress01 < 1.f)
        {
            continue;
        }

        // Stage complete -> advance state
        if (M.state == EDispatchMissionState::Traveling)
        {
            SetMissionState(M, EDispatchMissionState::Working);
        }
        else if (M.state == EDispatchMissionState::Working)
        {
            SetMissionState(M, EDispatchMissionState::Returning);
        }
        else if (M.state == EDispatchMissionState::Returning)
        {
            SetMissionState(M, EDispatchMissionState::Completed);
            activeMissions.RemoveAt(i);
        }
    }
}

ADispatchMissionSiteActor* UDispatchMissionManagerComponent::PickMissionSite(EDispatchMissionLocation Location, FRandomStream& Rng) const
{
    if (!bUseMissionSites)
    {
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    TArray<ADispatchMissionSiteActor*> Candidates;
    for (TActorIterator<ADispatchMissionSiteActor> It(World); It; ++It)
    {
        ADispatchMissionSiteActor* Site = *It;
        if (!Site)
        {
            continue;
        }

        if (Location == EDispatchMissionLocation::Any || Site->GetMissionLocation() == Location || Site->GetMissionLocation() == EDispatchMissionLocation::Any)
        {
            Candidates.Add(Site);
        }
    }

    if (Candidates.Num() == 0)
    {
        return nullptr;
    }

    const int32 Index = Rng.RandRange(0, Candidates.Num() - 1);
    return Candidates[Index];
}

FVector UDispatchMissionManagerComponent::GetFallbackWorldLocation(FRandomStream& Rng) const
{
    // Simple fallback around origin if no sites are placed.
    const float Radius = 2500.f;
    const FVector Dir3 = Rng.VRand();
    FVector2D Rand2(Dir3.X, Dir3.Y);
    Rand2 = Rand2.GetSafeNormal();
    return FVector(Rand2.X, Rand2.Y, 0.f) * Radius;
}

FDispatchMissionDifficulty UDispatchMissionManagerComponent::RollDifficulty(UDispatchMissionDefinition* Def, FRandomStream& Rng) const
{
    FDispatchMissionDifficulty D;

    if (!Def)
    {
        D.monsterChance10 = Rng.RandRange(0, 10);
        D.lootChance10 = Rng.RandRange(0, 10);
        D.complicationChance10 = Rng.RandRange(0, 10);
        return D;
    }

    auto Roll10 = [&](const FIntPoint& Range)
    {
        const int32 MinV = FMath::Clamp(Range.X, 0, 10);
        const int32 MaxV = FMath::Clamp(FMath::Max(MinV, Range.Y), 0, 10);
        return Rng.RandRange(MinV, MaxV);
    };

    D.monsterChance10 = Roll10(Def->monsterChanceRange10);
    D.lootChance10 = Roll10(Def->lootChanceRange10);
    D.complicationChance10 = Roll10(Def->complicationChanceRange10);
    return D;
}

void UDispatchMissionManagerComponent::SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState)
{
    Mission.state = NewState;
    Mission.stageTime = 0.f;
    Mission.stageProgress01 = 0.f;

    // Roll next stage duration from definition
    if (Mission.definition)
    {
        FRandomStream Rng(Mission.seed + (int32)NewState * 101);

        auto RollRange = [&](const FVector2D& Range)
        {
            const float MinV = FMath::Max(0.1f, Range.X);
            const float MaxV = FMath::Max(MinV, Range.Y);
            return Rng.FRandRange(MinV, MaxV);
        };

        if (NewState == EDispatchMissionState::Traveling)
        {
            Mission.stageDuration = RollRange(Mission.definition->travelDurationRangeSec);
        }
        else if (NewState == EDispatchMissionState::Working)
        {
            Mission.stageDuration = RollRange(Mission.definition->workDurationRangeSec);
        }
        else if (NewState == EDispatchMissionState::Returning)
        {
            Mission.stageDuration = RollRange(Mission.definition->returnDurationRangeSec);
        }
        else
        {
            Mission.stageDuration = 0.1f;
        }
    }
    else
    {
        Mission.stageDuration = 1.f;
    }

    OnMissionStateChanged.Broadcast(Mission.missionId, NewState);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Mission state | MissionId=%s | State=%d"),
        *Mission.missionId.ToString(), (int32)NewState);
}

#pragma endregion INTERNAL
