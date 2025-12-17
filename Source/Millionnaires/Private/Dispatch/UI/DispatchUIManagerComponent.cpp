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
#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
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
}

void UDispatchUIManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindFromComponents();

    if (cursorRadialWidget)
    {
        cursorRadialWidget->RemoveFromParent();
        cursorRadialWidget = nullptr;
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

    // Keep a game+ui input mode so world clicks remain possible (we'll add world-space notifications next).
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

    FInputModeGameOnly Mode;
    PC->SetInputMode(Mode);

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

