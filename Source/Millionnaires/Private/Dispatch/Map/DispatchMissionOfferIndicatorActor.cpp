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
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#pragma region LIFECYCLE

ADispatchMissionOfferIndicatorActor::ADispatchMissionOfferIndicatorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    widgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    RootComponent = widgetComponent;

    // Keep this widget in true World space (perspective + occlusion), but we will billboard it toward the camera.
    widgetComponent->SetWidgetSpace(EWidgetSpace::World);
    widgetComponent->SetDrawAtDesiredSize(true);
    widgetComponent->SetTwoSided(true);

    // Ensure bindings/events update reliably when rendered by a WidgetComponent.
    widgetComponent->SetManuallyRedraw(false);
    widgetComponent->SetRedrawTime(0.f);
    widgetComponent->SetTickWhenOffscreen(true);

    // Center pivot so the widget stays centered on its world anchor.
    widgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
}

void ADispatchMissionOfferIndicatorActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Keep the widget facing the player's camera while staying in World space.
    if (bBillboardToCamera)
    {
        billboardAccum += DeltaSeconds;

        if (billboardUpdateInterval <= 0.f || billboardAccum >= billboardUpdateInterval)
        {
            billboardAccum = 0.f;

            if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
            {
                FVector CamLoc;
                FRotator CamRot;
                PC->GetPlayerViewPoint(CamLoc, CamRot);

                const FVector SelfLoc = GetActorLocation();
                FRotator LookAt = (CamLoc - SelfLoc).Rotation();

                if (bBillboardYawOnly)
                {
                    LookAt.Pitch = 0.f;
                    LookAt.Roll = 0.f;
                }

                SetActorRotation(LookAt);
            }
        }
    }

    if (bTickUpdateWidget)
    {
        UpdateWidgetFromOffer();
    }
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

        // Not strictly required, but helps if the WidgetComponent uses a cached render.
        widgetComponent->RequestRedraw();
    }
}

#pragma endregion INTERNAL
