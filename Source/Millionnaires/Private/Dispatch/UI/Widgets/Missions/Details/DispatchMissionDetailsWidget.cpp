/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionDetailsWidget" - Source
 * Notes: Base widget for the mission details menu (pauses mission time while open).
 */
#include "Dispatch/UI/Widgets/Missions/Details/DispatchMissionDetailsWidget.h"

#include "Dispatch/Missions/DispatchMissionDefinition.h"

void UDispatchMissionDetailsWidget::SetFromOffer(const FGuid& InOfferId, const FDispatchMissionOffer& Offer)
{
    offerId = InOfferId;
    missionId.Invalidate();

    if (Offer.definition)
    {
        titleText = Offer.definition->title;
        descriptionText = Offer.definition->description;
        iconTexture = Offer.definition->icon;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;
    }

    BuildDifficultyEntriesFromDifficulty(Offer.difficulty);

    BP_OnDataUpdated();
    BP_OnDifficultyUpdated();
}

void UDispatchMissionDetailsWidget::SetFromMission(const FGuid& InMissionId, const FDispatchActiveMission& Mission)
{
    missionId = InMissionId;
    offerId.Invalidate();

    if (Mission.definition)
    {
        titleText = Mission.definition->title;
        descriptionText = Mission.definition->description;
        iconTexture = Mission.definition->icon;
    }
    else
    {
        titleText = FText::FromString(TEXT("Mission"));
        descriptionText = FText();
        iconTexture = nullptr;
    }

    BuildDifficultyEntriesFromDifficulty(Mission.difficulty);

    BP_OnDataUpdated();
    BP_OnDifficultyUpdated();
}

void UDispatchMissionDetailsWidget::RequestClose()
{
    OnCloseRequested.Broadcast();
}

#pragma region INTERNAL

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
