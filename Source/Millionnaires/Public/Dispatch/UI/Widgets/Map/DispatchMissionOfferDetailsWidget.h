/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferDetailsWidget" - Header
 * Notes: Base widget for mission details panel inside the map.
 */
#pragma once

#include "CoreMinimal.h"
#include "Dispatch/UI/Widgets/DispatchFadableWidget.h"
#include "DispatchMissionOfferDetailsWidget.generated.h"

class UButton;

USTRUCT(BlueprintType)
struct FDispatchMissionOfferSummary
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    FGuid offerId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    FText title;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    int32 missionType = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    int32 monsterChance10 = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    int32 lootChance10 = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dispatch|Map|Details")
    int32 complicationChance10 = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDispatchOfferActionRequested, const FGuid&, OfferId);

/**
 * Details panel for a selected mission offer.
 * Blueprint implements visuals; this base provides consistent accept/decline events.
 */
UCLASS()
class MILLIONNAIRES_API UDispatchMissionOfferDetailsWidget : public UDispatchFadableWidget
{
    GENERATED_BODY()

public:
#pragma region API

    /** Sets the offer summary to display. */
    UFUNCTION(BlueprintCallable, Category="Dispatch|Map|Details")
    void SetOfferSummary(const FDispatchMissionOfferSummary& InSummary);

#pragma endregion API

#pragma region EVENTS

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Details|Events")
    FDispatchOfferActionRequested OnAcceptRequested;

    UPROPERTY(BlueprintAssignable, Category="Dispatch|Map|Details|Events")
    FDispatchOfferActionRequested OnDeclineRequested;

#pragma endregion EVENTS

protected:
#pragma region UUSERWIDGET

    virtual void NativeConstruct() override;

#pragma endregion UUSERWIDGET

#pragma region BINDINGS

    UPROPERTY(meta=(BindWidgetOptional))
    UButton* acceptButton = nullptr;

    UPROPERTY(meta=(BindWidgetOptional))
    UButton* declineButton = nullptr;

#pragma endregion BINDINGS

#pragma region STATE

    UPROPERTY(BlueprintReadOnly, Category="Dispatch|Map|Details")
    FDispatchMissionOfferSummary summary;

#pragma endregion STATE

#pragma region BP_HOOKS

    UFUNCTION(BlueprintImplementableEvent, Category="Dispatch|Map|Details")
    void HandleSummaryApplied(const FDispatchMissionOfferSummary& InSummary);

#pragma endregion BP_HOOKS

private:
#pragma region INTERNAL

    UFUNCTION()
    void HandleAcceptClicked();

    UFUNCTION()
    void HandleDeclineClicked();

#pragma endregion INTERNAL
};
