/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchPlayerController"
 * Notes: Implements camera switching fade and cursor-driven radial UI for the Dispatch scene.
 */

#include "Dispatch/DispatchPlayerController.h"

#include "Dispatch/DispatchCameraManagerComponent.h"
#include "Dispatch/DispatchCursorComponent.h"
#include "Dispatch/DispatchCursorRadialWidget.h"
#include "Dispatch/DispatchCameraSpot.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "Camera/PlayerCameraManager.h"

ADispatchPlayerController::ADispatchPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;

    DefaultMouseCursor = EMouseCursor::Default;

    // Create sub components
    CameraManagerComponent = CreateDefaultSubobject<UDispatchCameraManagerComponent>(TEXT("DispatchCameraManager"));
    CursorComponent = CreateDefaultSubobject<UDispatchCursorComponent>(TEXT("DispatchCursor"));
}

#pragma region LIFECYCLE

void ADispatchPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Apply the MappingContext to the Enhanced Input subsystem.
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            if (DispatchIMC)
            {
                Subsystem->AddMappingContext(DispatchIMC, 0);
            }
        }
    }

    // Ensure we still receive keyboard/mouse input while the cursor is visible (Dispatch is UI-heavy).
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        SetInputMode(InputMode);
    }

    // Bind to camera manager delegate if available.
    if (CameraManagerComponent)
    {
        CameraManagerComponent->OnActiveCameraChanged.AddDynamic(this, &ADispatchPlayerController::HandleActiveCameraChanged);

        // In case the manager activated a camera before we bound the delegate, sync it now.
        if (ADispatchCameraSpot* Active = CameraManagerComponent->GetActiveCamera())
        {
            HandleActiveCameraChanged(Active);
        }
    }

    // Create and hook cursor radial widget.
    if (CursorRadialWidgetClass)
    {
        CursorRadialWidgetInstance = CreateWidget<UDispatchCursorRadialWidget>(this, CursorRadialWidgetClass);
        if (CursorRadialWidgetInstance)
        {
            CursorRadialWidgetInstance->AddToViewport(10);
            CursorRadialWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Bind to cursor progress delegate so we can drive the radial widget.
    if (CursorComponent)
    {
        CursorComponent->OnHoverProgress.AddDynamic(this, &ADispatchPlayerController::HandleHoverProgress);
    }
}

void ADispatchPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Dispatch] No InputComponent on DispatchPlayerController."));
        return;
    }

    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent))
    {
        UE_LOG(LogTemp, Log, TEXT("[Dispatch] SetupInputComponent | IMC=%s | Zoom=%s | Click=%s"),
            *GetNameSafe(DispatchIMC),
            *GetNameSafe(ZoomAction),
            *GetNameSafe(ClickAction)
        );

        if (CameraLeftAction)
        {
            EI->BindAction(CameraLeftAction, ETriggerEvent::Started, this, &ADispatchPlayerController::CycleCameraLeft);
        }

        if (CameraRightAction)
        {
            EI->BindAction(CameraRightAction, ETriggerEvent::Started, this, &ADispatchPlayerController::CycleCameraRight);
        }

        if (ZoomAction)
        {
            // Some IA trigger setups only emit Triggered (not Started). Bind both for robustness.
            EI->BindAction(ZoomAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleZoomPressed);
            EI->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ADispatchPlayerController::HandleZoomPressed);
            EI->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ADispatchPlayerController::HandleZoomReleased);
            EI->BindAction(ZoomAction, ETriggerEvent::Canceled, this, &ADispatchPlayerController::HandleZoomReleased);
        }

        if (ClickAction)
        {
            EI->BindAction(ClickAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleCursorClick);
        }
    }
}

#pragma endregion LIFECYCLE

#pragma region CAMERA_API

void ADispatchPlayerController::CycleCameraLeft()
{
    if (!CameraManagerComponent || bIsCameraFading)
    {
        return;
    }

    StartCameraFadeSequence(EDispatchPendingCameraSwitch::Left);
}

void ADispatchPlayerController::CycleCameraRight()
{
    if (!CameraManagerComponent || bIsCameraFading)
    {
        return;
    }

    StartCameraFadeSequence(EDispatchPendingCameraSwitch::Right);
}

void ADispatchPlayerController::HandleZoomPressed()
{
    if (CameraManagerComponent)
    {
        CameraManagerComponent->SetWantsZoom(true);
    }
}

void ADispatchPlayerController::HandleZoomReleased()
{
    if (CameraManagerComponent)
    {
        CameraManagerComponent->SetWantsZoom(false);
    }
}

#pragma endregion CAMERA_API

#pragma region CURSOR_API

void ADispatchPlayerController::HandleCursorClick()
{
    if (!CursorComponent)
    {
        return;
    }

    // Cursor handles generic interaction (interfaces, tags, etc).
    CursorComponent->HandleClick();
}

#pragma endregion CURSOR_API

#pragma region INTERNAL_CALLBACKS

void ADispatchPlayerController::HandleActiveCameraChanged(ADispatchCameraSpot* NewCamera)
{
    CurrentCameraSpot = NewCamera;
}

void ADispatchPlayerController::HandleHoverProgress(float Progress, bool bIsHoveringCharacter)
{
    if (!CursorRadialWidgetInstance)
    {
        return;
    }

    if (Progress <= KINDA_SMALL_NUMBER)
    {
        CursorRadialWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    FVector2D MousePos;
    if (GetMousePosition(MousePos.X, MousePos.Y))
    {
        CursorRadialWidgetInstance->SetScreenPosition(MousePos);
    }

    CursorRadialWidgetInstance->SetProgress(Progress);
    CursorRadialWidgetInstance->SetVisibility(ESlateVisibility::Visible);
}

void ADispatchPlayerController::StartCameraFadeSequence(EDispatchPendingCameraSwitch SwitchDirection)
{
    if (!PlayerCameraManager)
    {
        return;
    }

    PendingCameraSwitch = SwitchDirection;
    bIsCameraFading = true;

    // Fade to black, hold when finished until we manually fade-in.
    PlayerCameraManager->StartCameraFade(
        0.f,
        1.f,
        CameraFadeOutDuration,
        FLinearColor::Black,
        false,
        true
    );

    GetWorldTimerManager().ClearTimer(CameraFadeTimerHandle);
    GetWorldTimerManager().SetTimer(
        CameraFadeTimerHandle,
        this,
        &ADispatchPlayerController::HandleCameraFadeOutFinished,
        CameraFadeOutDuration,
        false
    );
}

void ADispatchPlayerController::HandleCameraFadeOutFinished()
{
    if (CameraManagerComponent)
    {
        if (PendingCameraSwitch == EDispatchPendingCameraSwitch::Left)
        {
            CameraManagerComponent->CycleCameraLeft();
        }
        else if (PendingCameraSwitch == EDispatchPendingCameraSwitch::Right)
        {
            CameraManagerComponent->CycleCameraRight();
        }
    }

    PendingCameraSwitch = EDispatchPendingCameraSwitch::None;

    if (PlayerCameraManager)
    {
        // Fade from black back to normal, do not hold once finished.
        PlayerCameraManager->StartCameraFade(
            1.f,
            0.f,
            CameraFadeInDuration,
            FLinearColor::Black,
            false,
            false
        );
    }

    GetWorldTimerManager().ClearTimer(CameraFadeTimerHandle);
    GetWorldTimerManager().SetTimer(
        CameraFadeTimerHandle,
        this,
        &ADispatchPlayerController::HandleCameraFadeInFinished,
        CameraFadeInDuration,
        false
    );
}

void ADispatchPlayerController::HandleCameraFadeInFinished()
{
    bIsCameraFading = false;
    GetWorldTimerManager().ClearTimer(CameraFadeTimerHandle);
}

#pragma endregion INTERNAL_CALLBACKS
