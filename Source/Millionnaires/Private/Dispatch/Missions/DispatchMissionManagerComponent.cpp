/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionManagerComponent" - Source
 * Notes: Generates mission offers, handles time-limited acceptance, and simulates Dispatch missions.
 */
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"
#include "Dispatch/Map/DispatchMissionSiteActor.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"

#include "Components/Characters/CharacterStatsComponent.h"
#include "Data/CharacterDefinition.h"

#pragma region LIFECYCLE

UDispatchMissionManagerComponent::UDispatchMissionManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDispatchMissionManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    // Ensure we start with a scheduled offer.
    FRandomStream Rng(runSeed);
    ScheduleNextOffer(Rng);
}

void UDispatchMissionManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bMissionTimePaused)
    {
        return;
    }

    if (bAutoSpawnOffers)
    {
        timeUntilNextOffer -= DeltaTime;
        if (timeUntilNextOffer <= 0.f)
        {
            GenerateOffer();

            FRandomStream Rng(runSeed + currentDay * 101 + offerCounter * 17);
            ScheduleNextOffer(Rng);
        }
    }

    UpdateOfferTimers(DeltaTime);
    UpdateMissionSimulation(DeltaTime);
}

#pragma endregion LIFECYCLE

#pragma region API_RUN

void UDispatchMissionManagerComponent::StartNewRun()
{
    ResetState();

    currentDay = 1;

    FRandomStream Rng(runSeed);
    ScheduleNextOffer(Rng);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] StartNewRun | Day=%d | Seed=%d"), currentDay, runSeed);
}

void UDispatchMissionManagerComponent::AdvanceDay()
{
    currentDay = FMath::Max(1, currentDay + 1);
    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] AdvanceDay | Day=%d"), currentDay);
}

#pragma endregion API_RUN

#pragma region API_TIME

void UDispatchMissionManagerComponent::SetMissionTimePaused(bool bPaused)
{
    if (bMissionTimePaused == bPaused)
    {
        return;
    }

    bMissionTimePaused = bPaused;
    OnMissionTimePausedChanged.Broadcast(bMissionTimePaused);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] TimePaused=%s"), bMissionTimePaused ? TEXT("true") : TEXT("false"));
}

#pragma endregion API_TIME

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
    int32 Index = INDEX_NONE;
    for (int32 i = 0; i < offers.Num(); ++i)
    {
        if (offers[i].offerId == OfferId)
        {
            Index = i;
            break;
        }
    }

    if (Index == INDEX_NONE)
    {
        return false;
    }

    const FDispatchMissionOffer Offer = offers[Index];

    // Validate agents.
    if (!Offer.definition)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (null definition)."));
        return false;
    }

    // Busy agents check
    for (APawn* P : SelectedAgents)
    {
        if (IsAgentBusy(P))
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (agent already busy). Pawn=%s"), *GetNameSafe(P));
            return false;
        }
    }

    // Definition constraints
    const FDispatchMissionSuccessModel& Model = Offer.definition->successModel;

    if (Offer.missionMode == EDispatchMissionMode::FPS)
    {
        if (SelectedAgents.Num() != 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (FPS needs exactly 1 agent)."));
            return false;
        }
    }
    else
    {
        const int32 MinAgents = FMath::Max(1, Model.minAgents);
        const int32 MaxAgents = FMath::Max(MinAgents, Model.maxAgents);

        if (SelectedAgents.Num() < MinAgents || SelectedAgents.Num() > MaxAgents)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] AcceptOffer rejected (needs %d..%d agents)."), MinAgents, MaxAgents);
            return false;
        }
    }

    FDispatchActiveMission Mission;
    Mission.missionId = FGuid::NewGuid();
    Mission.sourceOfferId = Offer.offerId;
    Mission.definition = Offer.definition;
    Mission.missionMode = Offer.missionMode;
    Mission.missionLocation = Offer.missionLocation;
    Mission.locationActor = Offer.locationActor;
    Mission.worldLocation = Offer.worldLocation;
    Mission.difficulty = Offer.difficulty;
    Mission.seed = Offer.seed;

    for (APawn* P : SelectedAgents)
    {
        if (!P)
        {
            continue;
        }

        Mission.assignedAgents.Add(P);

        if (UCharacterStatsComponent* Stats = P->FindComponentByClass<UCharacterStatsComponent>())
        {
            if (UCharacterDefinition* Def = Stats->GetCharacterDefinition())
            {
                Mission.assignedCharacters.AddUnique(Def);
            }
        }
    }

    // Compute success chance at accept time.
    FDispatchMissionSuccessBreakdown Breakdown;
    float Chance01 = 0.f;
    if (ComputeOfferSuccessChance(OfferId, SelectedAgents, Chance01, Breakdown))
    {
        Mission.successChance01 = Chance01;
    }
    else
    {
        Mission.successChance01 = FMath::Clamp(Offer.definition->successModel.baseSuccessChance01, 0.f, 1.f);
    }


    SetMissionState(Mission, EDispatchMissionState::Accepted);
    activeMissions.Add(Mission);

    // Stop offer highlight on this site (we're transitioning to mission highlight).
    if (ADispatchMissionSiteActor* SiteActor = Offer.locationActor.Get())
    {
        SiteActor->SetOfferActive(false);
    }

    // Remove offer.
    offers.RemoveAt(Index);
    OnOfferAccepted.Broadcast(OfferId);
    OnOfferRemoved.Broadcast(OfferId);

    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer accepted | OfferId=%s -> MissionId=%s"),
           *OfferId.ToString(), *Mission.missionId.ToString());

    // Immediately start traveling stage for Dispatch mode.
    SetMissionState(activeMissions.Last(), EDispatchMissionState::Traveling);

    return true;
}bool UDispatchMissionManagerComponent::DeclineOffer(const FGuid& OfferId)
{
    // Stop highlight on the related site (player declined, no expiry pulse).
    for (const FDispatchMissionOffer& O : offers)
    {
        if (O.offerId == OfferId)
        {
            if (ADispatchMissionSiteActor* Site = O.locationActor.Get())
            {
                Site->SetOfferActive(false);
            }
            break;
        }
    }

    const int32 Before = offers.Num();
    offers.RemoveAll([&](const FDispatchMissionOffer& O) { return O.offerId == OfferId; });

    if (offers.Num() == Before)
    {
        return false;
    }

    OnOfferRemoved.Broadcast(OfferId);
    UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer declined | OfferId=%s"), *OfferId.ToString());
    return true;
}


#pragma endregion API_OFFERS

#pragma region API_MISSIONS

bool UDispatchMissionManagerComponent::TryGetActiveMission(const FGuid& MissionId, FDispatchActiveMission& OutMission) const
{
    for (const FDispatchActiveMission& M : activeMissions)
    {
        if (M.missionId == MissionId)
        {
            OutMission = M;
            return true;
        }
    }
    return false;
}

bool UDispatchMissionManagerComponent::TryGetActiveMissionFromOffer(const FGuid& SourceOfferId, FDispatchActiveMission& OutMission) const
{
    for (const FDispatchActiveMission& M : activeMissions)
    {
        if (M.sourceOfferId == SourceOfferId)
        {
            OutMission = M;
            return true;
        }
    }
    return false;
}


#pragma region API_SUCCESS

bool UDispatchMissionManagerComponent::IsAgentBusy(APawn* Agent) const
{
    if (!Agent)
    {
        return false;
    }

    for (const FDispatchActiveMission& M : activeMissions)
    {
        for (const TObjectPtr<APawn>& P : M.assignedAgents)
        {
            if (P == Agent)
            {
                return true;
            }
        }
    }

    return false;
}

static int32 GetDifficultyMetricValue10_Internal(const FDispatchMissionDifficulty& Difficulty, EDispatchMissionDifficultyMetric Metric)
{
    switch (Metric)
    {
        case EDispatchMissionDifficultyMetric::MonsterChance:
            return Difficulty.monsterChance10;
        case EDispatchMissionDifficultyMetric::LootChance:
            return Difficulty.lootChance10;
        case EDispatchMissionDifficultyMetric::ComplicationChance:
            return Difficulty.complicationChance10;
        default:
            return 0;
    }
}

static void EnsureDefaultSkillWeights(TMap<EDispatchMissionSkill, float>& InOutWeights)
{
    if (InOutWeights.Num() > 0)
    {
        return;
    }

    InOutWeights.Add(EDispatchMissionSkill::Combat, 1.0f);
}

bool UDispatchMissionManagerComponent::ComputeOfferSuccessChance(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents, float& OutChance01, FDispatchMissionSuccessBreakdown& OutBreakdown) const
{
    OutChance01 = 0.f;
    OutBreakdown = FDispatchMissionSuccessBreakdown();

    FDispatchMissionOffer Offer;
    if (!TryGetOffer(OfferId, Offer) || !Offer.definition)
    {
        return false;
    }

    const FDispatchMissionSuccessModel& Model = Offer.definition->successModel;

    // Base
    OutBreakdown.baseChance01 = FMath::Clamp(Model.baseSuccessChance01, 0.f, 1.f);

    // Difficulty penalty
    const int32 PenaltyValue10 = GetDifficultyMetricValue10_Internal(Offer.difficulty, Model.difficultyPenaltyMetric);
    OutBreakdown.difficultyPenalty01 = FMath::Max(0.f, (float)PenaltyValue10 * FMath::Max(0.f, Model.difficultyPenaltyPerPoint10));

    // Weights
    TMap<EDispatchMissionSkill, float> Weights = Model.skillWeights;
    EnsureDefaultSkillWeights(Weights);

    float WeightSum = 0.f;
    for (const auto& Kvp : Weights)
    {
        WeightSum += FMath::Max(0.f, Kvp.Value);
    }
    WeightSum = FMath::Max(WeightSum, KINDA_SMALL_NUMBER);

    // Per-agent bonuses
    for (APawn* P : SelectedAgents)
    {
        if (!P)
        {
            continue;
        }

        UCharacterStatsComponent* Stats = P->FindComponentByClass<UCharacterStatsComponent>();
        if (!Stats || !Stats->GetCharacterDefinition())
        {
            continue;
        }

        // Weighted skill score on 10
        float WeightedSkill10 = 0.f;
        for (const auto& Kvp : Weights)
        {
            const float W = FMath::Max(0.f, Kvp.Value);
            const int32 Skill10 = FMath::Clamp(Stats->GetSkillRating10(Kvp.Key), 0, 10);
            WeightedSkill10 += (float)Skill10 * W;
        }
        WeightedSkill10 /= WeightSum;

        OutBreakdown.skillBonus01 += WeightedSkill10 * FMath::Max(0.f, Model.perSkillPoint10ToChance01);

        // Affinity
        const float Affinity = Stats->GetLocationAffinityMultiplier(Offer.missionLocation);
        OutBreakdown.affinityBonus01 += (Affinity - 1.0f) * FMath::Max(0.f, Model.affinityBonusWeight);

        // Equipment/flat bonus placeholders
        OutBreakdown.equipmentBonus01 += Stats->GetEquipmentBonus01() * FMath::Max(0.f, Model.equipmentBonusWeight);
        OutBreakdown.equipmentBonus01 += Stats->GetFlatSuccessBonus01();
    }

    const float Raw = OutBreakdown.baseChance01 + OutBreakdown.skillBonus01 + OutBreakdown.affinityBonus01 + OutBreakdown.equipmentBonus01 - OutBreakdown.difficultyPenalty01;
    OutBreakdown.finalChance01 = FMath::Clamp(Raw, Model.clampMin01, Model.clampMax01);
    OutChance01 = OutBreakdown.finalChance01;

    return true;
}

#pragma endregion API_SUCCESS


#pragma endregion API_MISSIONS

#pragma region INTERNAL

void UDispatchMissionManagerComponent::ResetState()
{
    offers.Reset();
    activeMissions.Reset();
    timeUntilNextOffer = 3.f;
    bMissionTimePaused = false;
    offerCounter = 0;
}

void UDispatchMissionManagerComponent::ScheduleNextOffer(FRandomStream& Rng)
{
    const float Min = FMath::Min(offerIntervalRangeSec.X, offerIntervalRangeSec.Y);
    const float Max = FMath::Max(offerIntervalRangeSec.X, offerIntervalRangeSec.Y);
    timeUntilNextOffer = Rng.FRandRange(Min, Max);
}

void UDispatchMissionManagerComponent::GenerateOffer()
{
    if (availableDefinitions.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] No availableDefinitions set - cannot generate offers."));
        return;
    }

    ++offerCounter;

    // Try a few times to find a free site/location (1 mission per place).
    const int32 MaxAttempts = 8;
    for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
    {
        FRandomStream Rng(runSeed + currentDay * 7919 + offerCounter * 1543 + Attempt * 31);

        // Pick random definition.
        UDispatchMissionDefinition* Def = availableDefinitions[Rng.RandRange(0, availableDefinitions.Num() - 1)];
        if (!Def)
        {
            continue;
        }

        // Resolve site/world location.
        ADispatchMissionSiteActor* Site = bUseMissionSites ? PickMissionSite(Def->missionLocation, Rng) : nullptr;
        const FVector WorldLocation = Site ? Site->GetActorLocation() : GetFallbackWorldLocation(Rng);

        // Check if this place is already used by an offer or an active mission.
        const auto IsPlaceBusy = [&](ADispatchMissionSiteActor* InSite, EDispatchMissionLocation InLocation) -> bool
        {
            for (const FDispatchMissionOffer& O : offers)
            {
                if (InSite && O.locationActor == InSite)
                {
                    return true;
                }
                if (!InSite && O.missionLocation == InLocation)
                {
                    return true;
                }
            }

            for (const FDispatchActiveMission& M : activeMissions)
            {
                if (InSite && M.locationActor == InSite)
                {
                    return true;
                }
                if (!InSite && M.missionLocation == InLocation && (M.state != EDispatchMissionState::Completed && M.state != EDispatchMissionState::Failed && M.state != EDispatchMissionState::Aborted))
                {
                    return true;
                }
            }

            return false;
        };

        if (IsPlaceBusy(Site, Def->missionLocation))
        {
            continue;
        }

        FDispatchMissionOffer Offer;
        Offer.offerId = FGuid::NewGuid();
        Offer.definition = Def;
        Offer.missionMode = Def->missionMode;
        Offer.missionLocation = Def->missionLocation;
        Offer.dayCreated = currentDay;
        Offer.seed = Rng.RandRange(0, 999999);

        Offer.locationActor = Site;
        Offer.worldLocation = WorldLocation;
        Offer.difficulty = RollDifficulty(Def, Rng);

        const float MinTL = (Def->offerTimeLimitMinSec > 0.f) ? Def->offerTimeLimitMinSec : defaultOfferTimeLimitRangeSec.X;
        const float MaxTL = (Def->offerTimeLimitMaxSec > 0.f) ? Def->offerTimeLimitMaxSec : defaultOfferTimeLimitRangeSec.Y;

        Offer.timeLimitSec = (FMath::IsNearlyEqual(MinTL, MaxTL)) ? MinTL : Rng.FRandRange(FMath::Min(MinTL, MaxTL), FMath::Max(MinTL, MaxTL));
        Offer.timeRemainingSec = Offer.timeLimitSec;

        offers.Add(Offer);
        OnOfferAdded.Broadcast(Offer.offerId);

        if (ADispatchMissionSiteActor* SiteActor = Offer.locationActor.Get())
        {
            SiteActor->SetOfferActive(true);
        }

        UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer added | OfferId=%s | Def=%s | Loc=%d | Time=%.1fs"),
               *Offer.offerId.ToString(), *GetNameSafe(Def), (int32)Offer.missionLocation, Offer.timeLimitSec);

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[DispatchMissions] GenerateOffer aborted: no free mission site/location found."));
}

void UDispatchMissionManagerComponent::UpdateOfferTimers(float DeltaTime)
{
    if (offers.Num() <= 0)
    {
        return;
    }

    TArray<FGuid> Expired;
    for (FDispatchMissionOffer& O : offers)
    {
        O.timeRemainingSec -= DeltaTime;
        if (O.timeRemainingSec <= 0.f)
        {
            Expired.Add(O.offerId);
        }
    }

    if (Expired.Num() <= 0)
    {
        return;
    }

    for (const FGuid& Id : Expired)
    {
        // Capture the offer before removal so we can drive highlights (pulse) correctly.
        ADispatchMissionSiteActor* Site = nullptr;

        for (const FDispatchMissionOffer& O : offers)
        {
            if (O.offerId == Id)
            {
                Site = O.locationActor.Get();
                break;
            }
        }

        // Stop the offer highlight and play a short pulse (offer expired without accept).
        if (Site)
        {
            Site->SetOfferActive(false);
            Site->PlayOfferExpiredPulse();
        }

        offers.RemoveAll([&](const FDispatchMissionOffer& O) { return O.offerId == Id; });

        OnOfferExpired.Broadcast(Id);
        OnOfferRemoved.Broadcast(Id);

        UE_LOG(LogTemp, Log, TEXT("[DispatchMissions] Offer expired | OfferId=%s"), *Id.ToString());
    }
}

void UDispatchMissionManagerComponent::UpdateMissionSimulation(float DeltaTime)
{
    if (activeMissions.Num() <= 0)
    {
        return;
    }

    for (int32 i = activeMissions.Num() - 1; i >= 0; --i)
    {
        FDispatchActiveMission& M = activeMissions[i];

        if (M.state == EDispatchMissionState::Completed || M.state == EDispatchMissionState::Failed || M.state == EDispatchMissionState::Aborted)
        {
            continue;
        }

        // Stage tick
        M.stageTime += DeltaTime;
        M.stageProgress01 = (M.stageDuration > 0.f) ? FMath::Clamp(M.stageTime / M.stageDuration, 0.f, 1.f) : 1.f;

        OnMissionProgress.Broadcast(M.missionId, M.stageProgress01);

        if (M.stageTime < M.stageDuration)
        {
            continue;
        }

        // Advance state
        switch (M.state)
        {
        case EDispatchMissionState::Accepted:
        case EDispatchMissionState::Traveling:
            SetMissionState(M, EDispatchMissionState::Working);
            break;

        case EDispatchMissionState::Working:
            SetMissionState(M, EDispatchMissionState::Returning);
            break;

        case EDispatchMissionState::Returning:
        {
            // Roll success/failure based on computed chance.
            FRandomStream Rng(M.seed + 99991);
            const bool bSuccess = (Rng.FRand() <= FMath::Clamp(M.successChance01, 0.f, 1.f));

            SetMissionState(M, bSuccess ? EDispatchMissionState::Completed : EDispatchMissionState::Failed);
            break;
        }

        default:
            break;
        }
    }
}

ADispatchMissionSiteActor* UDispatchMissionManagerComponent::PickMissionSite(EDispatchMissionLocation Location, FRandomStream& Rng) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    TArray<ADispatchMissionSiteActor*> Exact;
    TArray<ADispatchMissionSiteActor*> Any;

    for (TActorIterator<ADispatchMissionSiteActor> It(World); It; ++It)
    {
        ADispatchMissionSiteActor* S = *It;
        if (!S) continue;

        if (S->GetMissionLocation() == Location && Location != EDispatchMissionLocation::Any)
        {
            Exact.Add(S);
        }
        else if (S->GetMissionLocation() == EDispatchMissionLocation::Any)
        {
            Any.Add(S);
        }
    }

    if (Exact.Num() > 0)
    {
        return Exact[Rng.RandRange(0, Exact.Num() - 1)];
    }
    if (Any.Num() > 0)
    {
        return Any[Rng.RandRange(0, Any.Num() - 1)];
    }

    // If no Any sites exist, pick nearest-ish by random.
    TArray<ADispatchMissionSiteActor*> All;
    for (TActorIterator<ADispatchMissionSiteActor> It(World); It; ++It)
    {
        All.Add(*It);
    }
    if (All.Num() <= 0)
    {
        return nullptr;
    }
    return All[Rng.RandRange(0, All.Num() - 1)];
}

FVector UDispatchMissionManagerComponent::GetFallbackWorldLocation(FRandomStream& Rng) const
{
    // Simple box around owner for now.
    const FVector Origin = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
    const FVector Extent(800.f, 800.f, 0.f);

    return Origin + FVector(
        Rng.FRandRange(-Extent.X, Extent.X),
        Rng.FRandRange(-Extent.Y, Extent.Y),
        0.f
    );
}

FDispatchMissionDifficulty UDispatchMissionManagerComponent::RollDifficulty(UDispatchMissionDefinition* Def, FRandomStream& Rng) const
{
    FDispatchMissionDifficulty D;

    if (!Def)
    {
        return D;
    }

    D.monsterChance10 = Rng.RandRange(FMath::Min(Def->monsterChanceRange10.X, Def->monsterChanceRange10.Y),
                                     FMath::Max(Def->monsterChanceRange10.X, Def->monsterChanceRange10.Y));

    D.lootChance10 = Rng.RandRange(FMath::Min(Def->lootChanceRange10.X, Def->lootChanceRange10.Y),
                                  FMath::Max(Def->lootChanceRange10.X, Def->lootChanceRange10.Y));

    D.complicationChance10 = Rng.RandRange(FMath::Min(Def->complicationChanceRange10.X, Def->complicationChanceRange10.Y),
                                          FMath::Max(Def->complicationChanceRange10.X, Def->complicationChanceRange10.Y));

    return D;
}

void UDispatchMissionManagerComponent::SetMissionState(FDispatchActiveMission& Mission, EDispatchMissionState NewState)
{
    Mission.state = NewState;
    Mission.stageTime = 0.f;
    Mission.stageProgress01 = 0.f;

    // Roll stage duration based on state and definition.
    const UDispatchMissionDefinition* Def = Mission.definition;
    FRandomStream Rng(Mission.seed + (int32)NewState * 97);

    FVector2D Range(1.f, 1.f);

    if (Def)
    {
        switch (NewState)
        {
        case EDispatchMissionState::Traveling:
            Range = Def->travelDurationRangeSec;
            break;
        case EDispatchMissionState::Working:
            Range = Def->workDurationRangeSec;
            break;
        case EDispatchMissionState::Returning:
            Range = Def->returnDurationRangeSec;
            break;
        default:
            Range = FVector2D(1.f, 1.f);
            break;
        }
    }

    const float Min = FMath::Min(Range.X, Range.Y);
    const float Max = FMath::Max(Range.X, Range.Y);
    Mission.stageDuration = (FMath::IsNearlyEqual(Min, Max)) ? Min : Rng.FRandRange(Min, Max);

    if (ADispatchMissionSiteActor* SiteActor = Mission.locationActor.Get())
    {
        SiteActor->SetMissionState(NewState);
    }

    OnMissionStateChanged.Broadcast(Mission.missionId, NewState);
}

#pragma endregion INTERNAL
