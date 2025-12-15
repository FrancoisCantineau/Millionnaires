/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchUIManagerComponent" - Source
 * Notes: Creates Dispatch widgets and binds to cursor/mission events.
 */
#include "Dispatch/UI/DispatchUIManagerComponent.h"

#include "Dispatch/Cursor/DispatchCursorComponent.h"
#include "Dispatch/UI/Widgets/DispatchCursorRadialWidget.h"
#include "Dispatch/UI/Widgets/DispatchMapWidget.h"
#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#pragma region LIFECYCLE

UDispatchUIManagerComponent::UDispatchUIManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDispatchUIManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    BindToComponents();
    CreateCursorRadialWidget();
}

void UDispatchUIManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindFromComponents();

    if (cursorRadialWidget)
    {
        cursorRadialWidget->RemoveFromParent();
        cursorRadialWidget = nullptr;
    }

    if (mapWidget)
    {
        mapWidget->RemoveFromParent();
        mapWidget = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

#pragma endregion LIFECYCLE

#pragma region API_MAP

void UDispatchUIManagerComponent::OpenMap()
{
    if (bIsMapOpen)
    {
        return;
    }

    bIsMapOpen = true;

    // Disable world interactions while map is open.
    if (cursorComponent.IsValid())
    {
        cursorComponent->SetWorldCursorEnabled(false);
    }
    if (cameraManager.IsValid())
    {
        cameraManager->SetSuspended(true);
    }

    // Switch input mode to UIOnly and focus the map widget.
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        bPrevShowMouseCursor = PC->bShowMouseCursor;
        PC->bShowMouseCursor = true;

        SetMapWidgetVisible(true);

        if (mapWidget)
        {
            FInputModeUIOnly Mode;
            Mode.SetWidgetToFocus(mapWidget->TakeWidget());
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(Mode);
        }
    }
    else
    {
        SetMapWidgetVisible(true);
    }

    OnMapVisibilityChanged.Broadcast(true);
}

void UDispatchUIManagerComponent::CloseMap()
{
    if (!bIsMapOpen)
    {
        return;
    }

    bIsMapOpen = false;

    // Fade out map.
    SetMapWidgetVisible(false);

    // Restore world interactions.
    if (cursorComponent.IsValid())
    {
        cursorComponent->SetWorldCursorEnabled(true);
    }
    if (cameraManager.IsValid())
    {
        cameraManager->SetSuspended(false);
    }

    // Restore input mode.
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        PC->bShowMouseCursor = bPrevShowMouseCursor;

        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);
    }

    OnMapVisibilityChanged.Broadcast(false);
}

void UDispatchUIManagerComponent::ToggleMap()
{
    bIsMapOpen ? CloseMap() : OpenMap();
}

#pragma endregion API_MAP

#pragma region INTERNAL_BINDINGS

void UDispatchUIManagerComponent::BindToComponents()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    cursorComponent = OwnerActor->FindComponentByClass<UDispatchCursorComponent>();
    if (cursorComponent.IsValid())
    {
        cursorComponent->OnHoverUIProgress.AddDynamic(this, &UDispatchUIManagerComponent::HandleHoverUIProgress);
        cursorComponent->OnActorClicked.AddDynamic(this, &UDispatchUIManagerComponent::HandleActorClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchUI] No DispatchCursorComponent found on owner."));
    }

    cameraManager = OwnerActor->FindComponentByClass<UDispatchCameraManagerComponent>();

    missionManager = OwnerActor->FindComponentByClass<UDispatchMissionManagerComponent>();
    if (missionManager.IsValid())
    {
        missionManager->OnOfferAdded.AddDynamic(this, &UDispatchUIManagerComponent::HandleOfferAdded);
    }
}

void UDispatchUIManagerComponent::UnbindFromComponents()
{
    if (cursorComponent.IsValid())
    {
        cursorComponent->OnHoverUIProgress.RemoveDynamic(this, &UDispatchUIManagerComponent::HandleHoverUIProgress);
        cursorComponent->OnActorClicked.RemoveDynamic(this, &UDispatchUIManagerComponent::HandleActorClicked);
        cursorComponent = nullptr;
    }

    if (missionManager.IsValid())
    {
        missionManager->OnOfferAdded.RemoveDynamic(this, &UDispatchUIManagerComponent::HandleOfferAdded);
        missionManager = nullptr;
    }
}

#pragma endregion INTERNAL_BINDINGS

#pragma region INTERNAL_WIDGETS

void UDispatchUIManagerComponent::CreateCursorRadialWidget()
{
    if (!cursorRadialWidgetClass)
    {
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    cursorRadialWidget = CreateWidget<UDispatchCursorRadialWidget>(PC, cursorRadialWidgetClass);
    if (!cursorRadialWidget)
    {
        return;
    }

    cursorRadialWidget->AddToViewport(cursorRadialZOrder);
    cursorRadialWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UDispatchUIManagerComponent::SetMapWidgetVisible(bool bVisible)
{
    if (!mapWidgetClass)
    {
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    if (!mapWidget)
    {
        mapWidget = CreateWidget<UDispatchMapWidget>(PC, mapWidgetClass);
        if (mapWidget)
        {
            mapWidget->AddToViewport(mapZOrder);

            // Close button / Escape closes the map.
            mapWidget->OnCloseRequested.AddDynamic(this, &UDispatchUIManagerComponent::CloseMap);

            // Inject mission manager so the map can display mission offers.
            mapWidget->SetMissionManager(missionManager.Get());

            // Map -> UI actions.
            mapWidget->OnOfferDeclineRequested.AddDynamic(this, &UDispatchUIManagerComponent::HandleOfferDeclinedFromMap);
            mapWidget->OnOfferAcceptRequested.AddDynamic(this, &UDispatchUIManagerComponent::HandleOfferAcceptedFromMap);
        }
    }

    if (mapWidget)
    {
        mapWidget->RequestVisible(bVisible);
    }
}

#pragma endregion INTERNAL_WIDGETS

#pragma region INTERNAL_CALLBACKS

void UDispatchUIManagerComponent::HandleHoverUIProgress(float Progress, FVector2D ScreenPos, bool bVisible)
{
    if (!cursorRadialWidget)
    {
        return;
    }

    if (!bVisible || Progress <= 0.f)
    {
        cursorRadialWidget->RequestVisible(false);
        cursorRadialWidget->SetProgress(0.f);
        return;
    }

    cursorRadialWidget->RequestVisible(true);
    cursorRadialWidget->SetScreenPosition(ScreenPos);
    cursorRadialWidget->SetProgress(Progress);
}

void UDispatchUIManagerComponent::HandleActorClicked(AActor* ClickedActor)
{
    if (!ClickedActor)
    {
        return;
    }

    if (ClickedActor->ActorHasTag(mapHologramTag))
    {
        ToggleMap();
    }
}

void UDispatchUIManagerComponent::HandleOfferAdded(const FGuid& OfferId)
{
    // Placeholder: later you can spawn a notification widget from here.
    UE_LOG(LogTemp, Log, TEXT("[DispatchUI] New mission offer: %s"), *OfferId.ToString());
}

#pragma endregion INTERNAL_CALLBACKS


bool UDispatchUIManagerComponent::TryAcceptOffer(const FGuid& OfferId, const TArray<APawn*>& SelectedAgents)
{
    if (!missionManager.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchUI] TryAcceptOffer failed: missionManager missing."));
        return false;
    }

    return missionManager->AcceptOffer(OfferId, SelectedAgents);
}
