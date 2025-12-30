/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionOfferIndicatorActor" - Source
 * Notes: World-space mission offer indicator (WidgetComponent) spawned on NotificationPoints.
 */
#include "Dispatch/Map/DispatchMissionOfferIndicatorActor.h"

#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionDefinition.h"
#include "Dispatch/Map/DispatchMissionOfferClickProxyComponent.h"
#include "Dispatch/UI/Widgets/Missions/World/DispatchMissionWorldNotificationWidget.h"

#include "Components/WidgetComponent.h"

#pragma region LIFECYCLE

ADispatchMissionOfferIndicatorActor::ADispatchMissionOfferIndicatorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    widgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    RootComponent = widgetComponent;

    widgetComponent->SetWidgetSpace(EWidgetSpace::World);
    widgetComponent->SetDrawAtDesiredSize(true);
    widgetComponent->SetTwoSided(true);
}

void ADispatchMissionOfferIndicatorActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bTickUpdateWidget)
    {
        return;
    }

    UpdateWidgetFromOffer();
}

#pragma endregion LIFECYCLE

#pragma region API

void ADispatchMissionOfferIndicatorActor::Initialize(UDispatchMissionManagerComponent* InMissionManager, const FGuid& InOfferId)
{
    missionManager = InMissionManager;
    offerId = InOfferId;

    // Ensure click proxy exists so the UI manager can open details when clicked.
    clickProxy = FindComponentByClass<UDispatchMissionOfferClickProxyComponent>();
    if (!clickProxy)
    {
        clickProxy = NewObject<UDispatchMissionOfferClickProxyComponent>(this, UDispatchMissionOfferClickProxyComponent::StaticClass(), NAME_None, RF_Transient);
        if (clickProxy)
        {
            AddInstanceComponent(clickProxy);
            clickProxy->RegisterComponent();
        }
    }

    if (clickProxy)
    {
        clickProxy->SetOffer(offerId, nullptr);
    }

    UpdateWidgetFromOffer();
}

#pragma endregion API

#pragma region INTERNAL

UDispatchMissionWorldNotificationWidget* ADispatchMissionOfferIndicatorActor::GetTypedWidget() const
{
    if (!widgetComponent)
    {
        return nullptr;
    }
    return Cast<UDispatchMissionWorldNotificationWidget>(widgetComponent->GetUserWidgetObject());
}

void ADispatchMissionOfferIndicatorActor::UpdateWidgetFromOffer()
{
    if (!missionManager.IsValid() || !offerId.IsValid())
    {
        return;
    }

    FDispatchMissionOffer Offer;
    const bool bFound = missionManager->TryGetOffer(offerId, Offer);

    if (!bFound)
    {
        if (bAutoDestroyIfOfferMissing)
        {
            Destroy();
        }
        return;
    }

    if (UDispatchMissionWorldNotificationWidget* W = GetTypedWidget())
    {
        W->SetFromOffer(Offer);
    }
}

#pragma endregion INTERNAL
