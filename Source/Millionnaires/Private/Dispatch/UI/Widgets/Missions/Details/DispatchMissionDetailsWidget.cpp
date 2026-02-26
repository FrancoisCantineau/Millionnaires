/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionDetailsWidget" - Source
 * Notes: Base widget for the mission details menu (pauses mission time while open).
 */
#include "Dispatch/UI/Widgets/Missions/Details/DispatchMissionDetailsWidget.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"

#include "Components/Characters/CharacterStatsComponent.h"
#include "Data/CharacterDefinition.h"
#include "GameFramework/Pawn.h"

void UDispatchMissionDetailsWidget::SetFromOffer(const FGuid& InOfferId, const FDispatchMissionOffer& Offer)
{
    offerId = InOfferId;
    missionId.Invalidate();

    bIsViewingOffer = true;
    currentDefinition = Offer.definition;
    currentLocation = Offer.missionLocation;
    currentDifficulty = Offer.difficulty;

    // Reset selection when switching offers.
    ClearSelectedAgents();

    if (Offer.definition)
    {
        titleText = Offer.definition->title;
        descriptionText = Offer.definition->description;
        iconTexture = Offer.definition->icon;

        minAgentsRequired = FMath::Max(1, Offer.definition->successModel.minAgents);
        maxAgentsAllowed = FMath::Max(minAgentsRequired, Offer.definition->successModel.maxAgents);

        // Default preview (no agents selected).
        successChance01 = FMath::Clamp(Offer.definition->successModel.baseSuccessChance01, 0.f, 1.f);
        successBreakdown = FDispatchMissionSuccessBreakdown();
        successBreakdown.baseChance01 = successChance01;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;

        minAgentsRequired = 1;
        maxAgentsAllowed = 1;
        successChance01 = 0.f;
        successBreakdown = FDispatchMissionSuccessBreakdown();
    }

    BuildDifficultyEntriesFromDifficulty(Offer.difficulty);

    BP_OnDataUpdated();
    BP_OnDifficultyUpdated();
    BP_OnAgentsUpdated();
    BP_OnSuccessUpdated();
}

void UDispatchMissionDetailsWidget::SetFromMission(const FGuid& InMissionId, const FDispatchActiveMission& Mission)
{
    missionId = InMissionId;
    offerId.Invalidate();

    bIsViewingOffer = false;
    currentDefinition = Mission.definition;
    currentLocation = Mission.missionLocation;
    currentDifficulty = Mission.difficulty;

    // Show assigned agents as selected.
    selectedAgents = Mission.assignedAgents;
    RebuildAgentEntries(Mission.assignedAgents);

    if (Mission.definition)
    {
        titleText = Mission.definition->title;
        descriptionText = Mission.definition->description;
        iconTexture = Mission.definition->icon;

        minAgentsRequired = FMath::Max(1, Mission.definition->successModel.minAgents);
        maxAgentsAllowed = FMath::Max(minAgentsRequired, Mission.definition->successModel.maxAgents);
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;

        minAgentsRequired = 1;
        maxAgentsAllowed = 1;
    }

    successChance01 = FMath::Clamp(Mission.successChance01, 0.f, 1.f);
    successBreakdown = FDispatchMissionSuccessBreakdown();
    successBreakdown.finalChance01 = successChance01;

    BuildDifficultyEntriesFromDifficulty(Mission.difficulty);

    BP_OnDataUpdated();
    BP_OnDifficultyUpdated();
    BP_OnAgentsUpdated();
    BP_OnSuccessUpdated();
}

void UDispatchMissionDetailsWidget::RequestClose()
{
    OnCloseRequested.Broadcast();
}


void UDispatchMissionDetailsWidget::SetAvailableAgents(const TArray<APawn*>& Agents)
{
    RebuildAgentEntries(Agents);

    // Keep selection in sync with entries.
    RecomputeSuccessPreview();

    BP_OnAgentsUpdated();
    BP_OnSuccessUpdated();
}

void UDispatchMissionDetailsWidget::SetAgentBusy(APawn* Agent, bool bBusy)
{
    if (!Agent)
    {
        return;
    }

    for (FDispatchSelectableAgentEntry& Entry : availableAgents)
    {
        if (Entry.agentPawn == Agent)
        {
            Entry.bIsBusy = bBusy;

            // If an agent becomes busy while selected, deselect it.
            if (bBusy && Entry.bIsSelected)
            {
                Entry.bIsSelected = false;
                selectedAgents.Remove(Agent);
                RecomputeSuccessPreview();
            }

            BP_OnAgentsUpdated();
            BP_OnSuccessUpdated();
            return;
        }
    }
}

void UDispatchMissionDetailsWidget::ToggleAgentSelected(APawn* Agent)
{
    if (!bIsViewingOffer || !Agent)
    {
        return;
    }

    for (FDispatchSelectableAgentEntry& Entry : availableAgents)
    {
        if (Entry.agentPawn != Agent)
        {
            continue;
        }

        if (Entry.bIsBusy)
        {
            return;
        }

        if (Entry.bIsSelected)
        {
            Entry.bIsSelected = false;
            selectedAgents.Remove(Agent);
        }
        else
        {
            if (selectedAgents.Num() >= maxAgentsAllowed)
            {
                return;
            }

            Entry.bIsSelected = true;
            selectedAgents.AddUnique(Agent);
        }

        RecomputeSuccessPreview();

        BP_OnAgentsUpdated();
        BP_OnSuccessUpdated();
        return;
    }
}

void UDispatchMissionDetailsWidget::ClearSelectedAgents()
{
    selectedAgents.Reset();

    for (FDispatchSelectableAgentEntry& Entry : availableAgents)
    {
        Entry.bIsSelected = false;
    }

    RecomputeSuccessPreview();

    BP_OnAgentsUpdated();
    BP_OnSuccessUpdated();
}

void UDispatchMissionDetailsWidget::RequestAcceptOffer()
{
    if (!bIsViewingOffer || !offerId.IsValid())
    {
        return;
    }

    if (selectedAgents.Num() < minAgentsRequired || selectedAgents.Num() > maxAgentsAllowed)
    {
        return;
    }

    TArray<APawn*> Out;
    Out.Reserve(selectedAgents.Num());
    for (const TObjectPtr<APawn>& P : selectedAgents)
    {
        if (P)
        {
            Out.Add(P.Get());
        }
    }

    OnAcceptRequested.Broadcast(offerId, Out);
}


#pragma region INTERNAL

void UDispatchMissionDetailsWidget::RebuildAgentEntries(const TArray<APawn*>& Agents)
{
    // Preserve existing selection.
    TSet<APawn*> SelectedSet;
    for (const TObjectPtr<APawn>& P : selectedAgents)
    {
        if (P)
        {
            SelectedSet.Add(P.Get());
        }
    }

    availableAgents.Reset();
    availableAgents.Reserve(Agents.Num());

    for (APawn* P : Agents)
    {
        if (!P)
        {
            continue;
        }

        FDispatchSelectableAgentEntry Entry;
        Entry.agentPawn = P;
        Entry.bIsSelected = SelectedSet.Contains(P);

        if (UCharacterStatsComponent* Stats = P->FindComponentByClass<UCharacterStatsComponent>())
        {
            Entry.displayName = Stats->GetDisplayName();
            Entry.portrait = Stats->GetPortrait();
            Entry.characterDefinition = Stats->GetCharacterDefinition();
            Entry.characterId = Stats->GetCharacterId();
        }
        else
        {
            Entry.displayName = FText::FromString(GetNameSafe(P));
            Entry.portrait = nullptr;
            Entry.characterDefinition = nullptr;
            Entry.characterId = NAME_None;
        }

        availableAgents.Add(Entry);
    }
}

void UDispatchMissionDetailsWidget::RecomputeSuccessPreview()
{
    // For missions we keep stored chance.
    if (!bIsViewingOffer)
    {
        return;
    }

    UDispatchMissionDefinition* Def = GetCurrentDefinition();
    if (!Def)
    {
        successChance01 = 0.f;
        successBreakdown = FDispatchMissionSuccessBreakdown();
        return;
    }

    const FDispatchMissionSuccessModel& Model = Def->successModel;

    successBreakdown = FDispatchMissionSuccessBreakdown();
    successBreakdown.baseChance01 = FMath::Clamp(Model.baseSuccessChance01, 0.f, 1.f);

    const int32 PenaltyValue10 = GetDifficultyMetricValue10(currentDifficulty, Model.difficultyPenaltyMetric);
    successBreakdown.difficultyPenalty01 = FMath::Max(0.f, (float)PenaltyValue10 * FMath::Max(0.f, Model.difficultyPenaltyPerPoint10));

    // Weights
    TMap<EDispatchMissionSkill, float> Weights = Model.skillWeights;
    if (Weights.Num() == 0)
    {
        Weights.Add(EDispatchMissionSkill::Combat, 1.0f);
    }

    float WeightSum = 0.f;
    for (const auto& Kvp : Weights)
    {
        WeightSum += FMath::Max(0.f, Kvp.Value);
    }
    WeightSum = FMath::Max(WeightSum, KINDA_SMALL_NUMBER);

    // Bonuses
    for (const TObjectPtr<APawn>& PPtr : selectedAgents)
    {
        APawn* P = PPtr.Get();
        if (!P)
        {
            continue;
        }

        UCharacterStatsComponent* Stats = P->FindComponentByClass<UCharacterStatsComponent>();
        if (!Stats || !Stats->GetCharacterDefinition())
        {
            continue;
        }

        float WeightedSkill10 = 0.f;
        for (const auto& Kvp : Weights)
        {
            const float W = FMath::Max(0.f, Kvp.Value);
            const int32 Skill10 = FMath::Clamp(Stats->GetSkillRating10(Kvp.Key), 0, 10);
            WeightedSkill10 += (float)Skill10 * W;
        }
        WeightedSkill10 /= WeightSum;

        successBreakdown.skillBonus01 += WeightedSkill10 * FMath::Max(0.f, Model.perSkillPoint10ToChance01);

        const float Affinity = Stats->GetLocationAffinityMultiplier(currentLocation);
        successBreakdown.affinityBonus01 += (Affinity - 1.0f) * FMath::Max(0.f, Model.affinityBonusWeight);

        successBreakdown.equipmentBonus01 += Stats->GetEquipmentBonus01() * FMath::Max(0.f, Model.equipmentBonusWeight);
        successBreakdown.equipmentBonus01 += Stats->GetFlatSuccessBonus01();
    }

    const float Raw = successBreakdown.baseChance01 + successBreakdown.skillBonus01 + successBreakdown.affinityBonus01 + successBreakdown.equipmentBonus01 - successBreakdown.difficultyPenalty01;
    successBreakdown.finalChance01 = FMath::Clamp(Raw, Model.clampMin01, Model.clampMax01);
    successChance01 = successBreakdown.finalChance01;
}

void UDispatchMissionDetailsWidget::BuildDifficultyEntriesFromDifficulty(const FDispatchMissionDifficulty& Difficulty)
{
    difficultyEntries.Reset();

    // If the designer didn't set any configs, build simple defaults.
    if (difficultyDisplayConfigs.Num() == 0)
    {
        FDispatchMissionDifficultyUIEntry Monster;
        Monster.metric = EDispatchMissionDifficultyMetric::MonsterChance;
        Monster.title = FText::FromString(TEXT("Monster Chance"));
        Monster.maxSlots = 10;
        Monster.current = FMath::Clamp(Difficulty.monsterChance10, 0, Monster.maxSlots);
        difficultyEntries.Add(Monster);

        FDispatchMissionDifficultyUIEntry Loot;
        Loot.metric = EDispatchMissionDifficultyMetric::LootChance;
        Loot.title = FText::FromString(TEXT("Loot Chance"));
        Loot.maxSlots = 10;
        Loot.current = FMath::Clamp(Difficulty.lootChance10, 0, Loot.maxSlots);
        difficultyEntries.Add(Loot);

        FDispatchMissionDifficultyUIEntry Comp;
        Comp.metric = EDispatchMissionDifficultyMetric::ComplicationChance;
        Comp.title = FText::FromString(TEXT("Complication"));
        Comp.maxSlots = 10;
        Comp.current = FMath::Clamp(Difficulty.complicationChance10, 0, Comp.maxSlots);
        difficultyEntries.Add(Comp);

        return;
    }

    for (const FDispatchMissionDifficultyUIConfig& Cfg : difficultyDisplayConfigs)
    {
        FDispatchMissionDifficultyUIEntry Entry;
        Entry.metric = Cfg.metric;
        Entry.icon = Cfg.icon;
        Entry.title = Cfg.title;
        Entry.maxSlots = FMath::Max(1, Cfg.maxSlots);
        Entry.slotBackground = Cfg.slotBackground;
        Entry.slotFill = Cfg.slotFill;

        const int32 Value10 = GetDifficultyMetricValue10(Difficulty, Cfg.metric);
        Entry.current = FMath::Clamp(Value10, 0, Entry.maxSlots);

        difficultyEntries.Add(Entry);
    }
}

int32 UDispatchMissionDetailsWidget::GetDifficultyMetricValue10(const FDispatchMissionDifficulty& Difficulty, EDispatchMissionDifficultyMetric Metric)
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

#pragma endregion INTERNAL
