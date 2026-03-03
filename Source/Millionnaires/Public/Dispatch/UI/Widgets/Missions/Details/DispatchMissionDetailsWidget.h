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
class APawn;

#pragma region DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDispatchMissionDetailsCloseRequested);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDispatchMissionDetailsAcceptRequested, const FGuid&, OfferId, const TArray<APawn*>&, SelectedAgents);

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

    /// <summary>Selectable agents for this mission (build your UI from this array).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Agents")
    TArray<FDispatchSelectableAgentEntry> availableAgents;

    /// <summary>Selected agent pawns.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Agents")
    TArray<TObjectPtr<APawn>> selectedAgents;

    /// <summary>Min agents required for accept (from definition success model).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Agents")
    int32 minAgentsRequired = 1;

    /// <summary>Max agents allowed for accept (from definition success model).</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Agents")
    int32 maxAgentsAllowed = 3;

    /// <summary>Computed success chance preview (0..1). For active missions this is the stored chance.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Success")
    float successChance01 = 0.f;

    /// <summary>Success breakdown for UI/debug.</summary>
    UPROPERTY(BlueprintReadOnly, Category="Dispatch|UI|MissionDetails|Success")
    FDispatchMissionSuccessBreakdown successBreakdown;

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

    /// <summary>Sets the list of available agents (pawns) that can be selected for this mission.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails|Agents")
    void SetAvailableAgents(const TArray<APawn*>& Agents);

    /// <summary>Marks a specific agent entry as busy (e.g., already assigned).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails|Agents")
    void SetAgentBusy(APawn* Agent, bool bBusy);

    /// <summary>Toggles selection for an agent (respects min/max).</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails|Agents")
    void ToggleAgentSelected(APawn* Agent);

    /// <summary>Clears all selected agents.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails|Agents")
    void ClearSelectedAgents();

    /// <summary>UI accept button should call this to request accepting the current offer.</summary>
    UFUNCTION(BlueprintCallable, Category="Dispatch|UI|MissionDetails")
    void RequestAcceptOffer();

#pragma endregion API

#pragma region EVENTS

    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|MissionDetails")
    FDispatchMissionDetailsCloseRequested OnCloseRequested;


    /// <summary>Raised when user clicks Accept in the details menu.</summary>
    UPROPERTY(BlueprintAssignable, Category="Dispatch|UI|MissionDetails")
    FDispatchMissionDetailsAcceptRequested OnAcceptRequested;

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

    /// <summary>Called after availableAgents/selectedAgents are updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|MissionDetails|Agents")
    void BP_OnAgentsUpdated();

    /// <summary>Called after successChance01/successBreakdown are updated.</summary>
    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|UI|MissionDetails|Success")
    void BP_OnSuccessUpdated();

#pragma endregion BP_EVENTS

private:
#pragma region INTERNAL

    /// <summary>Builds difficultyEntries from a rolled difficulty struct.</summary>
    void BuildDifficultyEntriesFromDifficulty(const FDispatchMissionDifficulty& Difficulty);

    /// <summary>Returns the rolled value (0..10) for a metric inside a difficulty struct.</summary>
    static int32 GetDifficultyMetricValue10(const FDispatchMissionDifficulty& Difficulty, EDispatchMissionDifficultyMetric Metric);

    /// <summary>Rebuilds availableAgents entries from selected pawns.</summary>
    void RebuildAgentEntries(const TArray<APawn*>& Agents);

    /// <summary>Recomputes success chance preview based on selection and the current offer/definition.</summary>
    void RecomputeSuccessPreview();

    /// <summary>Returns the mission definition currently displayed (offer or mission).</summary>
    UDispatchMissionDefinition* GetCurrentDefinition() const { return currentDefinition.Get(); }

#pragma endregion INTERNAL

private:
    /// <summary>Cached definition for current offer/mission view.</summary>
    TObjectPtr<UDispatchMissionDefinition> currentDefinition = nullptr;

    /// <summary>Cached location for current offer/mission view.</summary>
    EDispatchMissionLocation currentLocation = EDispatchMissionLocation::Any;

    /// <summary>Cached difficulty for current offer/mission view.</summary>
    FDispatchMissionDifficulty currentDifficulty;

    /// <summary>Whether we are currently viewing an offer (selection allowed).</summary>
    bool bIsViewingOffer = false;

};
