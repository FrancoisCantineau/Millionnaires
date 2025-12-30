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
#include "Dispatch/UI/Widgets/Missions/Details/DispatchMissionDetailsWidget.h"
#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Map/DispatchMissionOfferClickProxyComponent.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"

namespace
{
    static AActor* FindFirstActorWithTag(UWorld* World, const FName Tag)
    {
        if (!World || Tag.IsNone())
        {
            return nullptr;
        }

        for (TActorIterator<AActor> It(World); It; ++It)
        {
            if (It->ActorHasTag(Tag))
            {
                return *It;
            }
        }

        return nullptr;
    }
}

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
    EnsureMissionDetailsWidget();
}

void UDispatchUIManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindFromComponents();

    if (cursorRadialWidget)
    {
        cursorRadialWidget->RemoveFromParent();
        cursorRadialWidget = nullptr;
    }

    if (missionDetailsWidget)
    {
        missionDetailsWidget->RemoveFromParent();
        missionDetailsWidget = nullptr;
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

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    bIsMapOpen = true;
    OnMapVisibilityChanged.Broadcast(true);

    // Cache and enable mouse cursor.
    bPrevShowMouseCursor = PC->bShowMouseCursor;
    PC->bShowMouseCursor = true;

    // Enter map mode: suspend camera manager movement & disable slow-mo cursor logic (strategic view).
    if (cameraManager.IsValid())
    {
        cameraManager->SetSuspended(true);
    }

    if (cursorComponent.IsValid())
    {
        cursorComponent->SetWorldCursorEnabled(false);
    }

    // Switch view to aerial/isometric map camera.
    UWorld* World = GetWorld();
    mapViewActor = mapViewActor.IsValid() ? mapViewActor.Get() : FindFirstActorWithTag(World, mapViewActorTag);

    if (mapViewActor.IsValid())
    {
        previousViewTarget = PC->GetViewTarget();
        PC->SetViewTargetWithBlend(mapViewActor.Get(), mapViewBlendTime);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchUI] Map camera actor not found (Tag=%s)."), *mapViewActorTag.ToString());
    }

    // Keep a game+ui input mode so world clicks remain possible.
    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(Mode);
}

void UDispatchUIManagerComponent::CloseMap()
{
    if (!bIsMapOpen)
    {
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    bIsMapOpen = false;
    OnMapVisibilityChanged.Broadcast(false);

    // Restore view target.
    if (previousViewTarget.IsValid())
    {
        PC->SetViewTargetWithBlend(previousViewTarget.Get(), mapViewBlendTime);
        previousViewTarget.Reset();
    }

    // Restore cursor + input.
    PC->bShowMouseCursor = bPrevShowMouseCursor;

    // If mission details are open, keep Game+UI input; otherwise back to game only.
    if (bIsMissionDetailsOpen)
    {
        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(Mode);
    }
    else
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
    }

    if (cameraManager.IsValid())
    {
        cameraManager->SetSuspended(false);
    }

    if (cursorComponent.IsValid())
    {
        cursorComponent->SetWorldCursorEnabled(true);
    }
}

void UDispatchUIManagerComponent::ToggleMap()
{
    bIsMapOpen ? CloseMap() : OpenMap();
}

#pragma endregion API_MAP

#pragma region API_MISSION_DETAILS

void UDispatchUIManagerComponent::OpenMissionDetailsForOffer(const FGuid& OfferId)
{
    if (!OfferId.IsValid())
    {
        return;
    }

    EnsureMissionDetailsWidget();
    if (!missionDetailsWidget || !missionManager.IsValid())
    {
        return;
    }

    FDispatchMissionOffer Offer;
    if (!missionManager->TryGetOffer(OfferId, Offer))
    {
        return;
    }

    missionDetailsWidget->SetFromOffer(OfferId, Offer);
    missionDetailsWidget->SetVisibility(ESlateVisibility::Visible);

    bIsMissionDetailsOpen = true;

    // Pause mission time while menu is open.
    if (bPauseMissionTimeWhenDetailsOpen)
    {
        missionManager->SetMissionTimePaused(true);
    }

    // Ensure cursor + input mode.
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        PC->bShowMouseCursor = true;

        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(Mode);
    }
}

void UDispatchUIManagerComponent::OpenMissionDetailsForMission(const FGuid& MissionId)
{
    if (!MissionId.IsValid())
    {
        return;
    }

    EnsureMissionDetailsWidget();
    if (!missionDetailsWidget || !missionManager.IsValid())
    {
        return;
    }

    FDispatchActiveMission Mission;
    if (!missionManager->TryGetActiveMission(MissionId, Mission))
    {
        return;
    }

    missionDetailsWidget->SetFromMission(MissionId, Mission);
    missionDetailsWidget->SetVisibility(ESlateVisibility::Visible);

    bIsMissionDetailsOpen = true;

    if (bPauseMissionTimeWhenDetailsOpen)
    {
        missionManager->SetMissionTimePaused(true);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        PC->bShowMouseCursor = true;

        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(Mode);
    }
}

void UDispatchUIManagerComponent::CloseMissionDetails()
{
    if (!missionDetailsWidget)
    {
        return;
    }

    missionDetailsWidget->SetVisibility(ESlateVisibility::Hidden);
    bIsMissionDetailsOpen = false;

    // Unpause mission time if map is not forcing it (only details uses pause here).
    if (missionManager.IsValid() && bPauseMissionTimeWhenDetailsOpen)
    {
        missionManager->SetMissionTimePaused(false);
    }

    // Restore input mode depending on map state.
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        if (bIsMapOpen)
        {
            FInputModeGameAndUI Mode;
            Mode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(Mode);
            PC->bShowMouseCursor = true;
        }
        else
        {
            FInputModeGameOnly Mode;
            PC->SetInputMode(Mode);
            PC->bShowMouseCursor = bPrevShowMouseCursor;
        }
    }
}

#pragma endregion API_MISSION_DETAILS

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

    cursorRadialWidget = CreateWidget<UDispatchCursorRadialWidget>(PC, cursorRadialWidgetClass.Get());
    if (!cursorRadialWidget)
    {
        return;
    }

    cursorRadialWidget->AddToViewport(cursorRadialZOrder);
    cursorRadialWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UDispatchUIManagerComponent::EnsureMissionDetailsWidget()
{
    if (missionDetailsWidget || !missionDetailsWidgetClass)
    {
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    // NOTE: CreateWidget expects a TSubclassOf<UUserWidget>. UDispatchMissionDetailsWidget derives from UUserWidget
    // so this is valid as long as the class is complete here (we included its header).
    missionDetailsWidget = CreateWidget<UDispatchMissionDetailsWidget>(PC, missionDetailsWidgetClass.Get());
    if (!missionDetailsWidget)
    {
        return;
    }

    missionDetailsWidget->OnCloseRequested.AddDynamic(this, &UDispatchUIManagerComponent::HandleDetailsCloseRequested);
    missionDetailsWidget->AddToViewport(missionDetailsZOrder);
    missionDetailsWidget->SetVisibility(ESlateVisibility::Hidden);
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
        cursorRadialWidget->SetVisibility(ESlateVisibility::Hidden);
        cursorRadialWidget->SetProgress(0.f);
        return;
    }

    cursorRadialWidget->SetVisibility(ESlateVisibility::Visible);
    cursorRadialWidget->SetScreenPosition(ScreenPos);
    cursorRadialWidget->SetProgress(Progress);
}

void UDispatchUIManagerComponent::HandleActorClicked(AActor* ClickedActor)
{
    if (!ClickedActor)
    {
        return;
    }

    // Map hologram toggles map view.
    if (ClickedActor->ActorHasTag(mapHologramTag))
    {
        ToggleMap();
        return;
    }

    // If clicked actor has a click proxy, open mission details.
    if (UDispatchMissionOfferClickProxyComponent* Proxy = ClickedActor->FindComponentByClass<UDispatchMissionOfferClickProxyComponent>())
    {
        const EDispatchMissionClickProxyKind Kind = Proxy->GetKind();
        if (Kind == EDispatchMissionClickProxyKind::Offer && Proxy->GetOfferId().IsValid())
        {
            OpenMissionDetailsForOffer(Proxy->GetOfferId());
            return;
        }
        if (Kind == EDispatchMissionClickProxyKind::Mission && Proxy->GetMissionId().IsValid())
        {
            OpenMissionDetailsForMission(Proxy->GetMissionId());
            return;
        }
    }
}

void UDispatchUIManagerComponent::HandleOfferAdded(const FGuid& OfferId)
{
    UE_LOG(LogTemp, Log, TEXT("[DispatchUI] New mission offer: %s"), *OfferId.ToString());
}

void UDispatchUIManagerComponent::HandleDetailsCloseRequested()
{
    CloseMissionDetails();
}

#pragma endregion INTERNAL_CALLBACKS
