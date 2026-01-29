/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionDetailsWidget" - Header
 * Notes: Base widget for the mission details menu (pauses mission time while open).
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dispatch/Missions/DispatchMissionTypes.h"
#include "DispatchMissionDetailsWidget.generated.h"

class UTexture2D;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDispatchMissionDetailsCloseRequested);

#pragma endregion DELEGATES

/**
 * Mission details widget shown when clicking a world notification or highlighted actor.
 * Create a WBP inheriting from this class and bind your UI to the exposed variables:
 * - titleText / descriptionText / iconTexture
 * - offerId / missionId (optional)
 */
UCLASS(Abstract, BlueprintType)
class MILLIONNAIRES_API UDispatchMissionDetailsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
#pragma region DATA_BINDING

    /// <summary>Offer id being displayed (if any).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails")
    FGuid offerId;

    /// <summary>Mission id being displayed (if any).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails")
    FGuid missionId;

    /// <summary>Title.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails")
    FText titleText;

    /// <summary>Description.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails")
    FText descriptionText;

    /// <summary>Icon.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails")
    TObjectPtr<UTexture2D> iconTexture = nullptr;

    /// <summary>Difficulty rows computed from the current offer/mission. Build your UI from this array.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TArray<FDispatchMissionDifficultyUIEntry> difficultyEntries;

#pragma endregion DATA_BINDING

public:
#pragma region DIFFICULTY_CONFIG

    /// <summary>
    /// UI config describing how each difficulty metric should be displayed.
    /// Set this in your WBP defaults to assign icons/titles/slot visuals.
    /// </summary>
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Difficulty")
    TArray<FDispatchMissionDifficultyUIConfig> difficultyDisplayConfigs;

#pragma endregion DIFFICULTY_CONFIG

public:
#pragma region API

    /// <summary>Sets this widget from an offer. Calls BP_OnDataUpdated().</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails")
    void SetFromOffer(const FGuid& InOfferId, const FDispatchMissionOffer& Offer);

    /// <summary>Sets this widget from an active mission. Calls BP_OnDataUpdated().</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails")
    void SetFromMission(const FGuid& InMissionId, const FDispatchActiveMission& Mission);

    /// <summary>UI button should call this to request closing the menu.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails")
    void RequestClose();

#pragma endregion API

#pragma region EVENTS

    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|MissionDetails")
    FDispatchMissionDetailsCloseRequested OnCloseRequested;

#pragma endregion EVENTS

protected:
#pragma region BP_EVENTS

    /// <summary>Called after SetFromOffer / SetFromMission updates the exposed variables.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|MissionDetails")
    void BP_OnDataUpdated();

    /// <summary>Called after difficultyEntries gets rebuilt.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|MissionDetails|Difficulty")
    void BP_OnDifficultyUpdated();

#pragma endregion BP_EVENTS

private:
#pragma region INTERNAL

    /// <summary>Builds difficultyEntries from a rolled difficulty struct.</summary>
    void BuildDifficultyEntriesFromDifficulty(const FDispatchMissionDifficulty& Difficulty);

    /// <summary>Returns the rolled value (0..10) for a metric inside a difficulty struct.</summary>
    static int32 GetDifficultyMetricValue10(const FDispatchMissionDifficulty& Difficulty, EDispatchMissionDifficultyMetric Metric);

#pragma endregion INTERNAL
};
