/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchPlayerController" - Source
 * Notes: Input routing for Dispatch mode (camera/cursor/ui/missions).
 */
#include "Dispatch/DispatchPlayerController.h"

#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Dispatch/Cursor/DispatchCursorComponent.h"
#include "Dispatch/UI/DispatchUIManagerComponent.h"
#include "Dispatch/Missions/DispatchMissionManagerComponent.h"
#include "Dispatch/Map/DispatchMissionWorldIndicatorComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"

#pragma region LIFECYCLE

ADispatchPlayerController::ADispatchPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // Prevent Unreal from overriding our Dispatch camera view target with the default Pawn.
    bAutoManageActiveCameraTarget = false;

    cameraManagerComponent = CreateDefaultSubobject<UDispatchCameraManagerComponent>(TEXT("BPC_DispatchCameraManager"));
    cursorComponent = CreateDefaultSubobject<UDispatchCursorComponent>(TEXT("BPC_DispatchCursor"));
    uiManagerComponent = CreateDefaultSubobject<UDispatchUIManagerComponent>(TEXT("BPC_DispatchUIManager"));
    missionManagerComponent = CreateDefaultSubobject<UDispatchMissionManagerComponent>(TEXT("BPC_DispatchMissionManager"));
    worldIndicatorComponent = CreateDefaultSubobject<UDispatchMissionWorldIndicatorComponent>(TEXT("BPC_DispatchWorldIndicators"));
}

void ADispatchPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Enhanced Input mapping context.
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (dispatchIMC)
            {
                Subsystem->AddMappingContext(dispatchIMC, 0);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[DispatchPC] dispatchIMC is not set."));
            }
        }
    }

    // Ensure we still receive input while cursor is visible.
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        SetInputMode(InputMode);
    }

    // Force-apply the startup Dispatch camera on next tick (after possession/automanage).
    GetWorldTimerManager().SetTimerForNextTick(this, &ADispatchPlayerController::ForceApplyInitialDispatchCamera);
}

void ADispatchPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchPC] EnhancedInputComponent missing."));
        return;
    }

    if (nextCameraAction)
    {
        EI->BindAction(nextCameraAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleNextCamera);
    }

    if (prevCameraAction)
    {
        EI->BindAction(prevCameraAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandlePrevCamera);
    }

    if (zoomAction)
    {
        // Robust binding: some trigger setups emit Triggered but not Started.
        EI->BindAction(zoomAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleZoomPressed);
        EI->BindAction(zoomAction, ETriggerEvent::Triggered, this, &ADispatchPlayerController::HandleZoomPressed);
        EI->BindAction(zoomAction, ETriggerEvent::Completed, this, &ADispatchPlayerController::HandleZoomReleased);
        EI->BindAction(zoomAction, ETriggerEvent::Canceled, this, &ADispatchPlayerController::HandleZoomReleased);
    }

    if (clickAction)
    {
        EI->BindAction(clickAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleClick);
    }

    UE_LOG(LogTemp, Log, TEXT("[DispatchPC] SetupInputComponent | IMC=%s | Zoom=%s | Click=%s"),
        *GetNameSafe(dispatchIMC),
        *GetNameSafe(zoomAction),
        *GetNameSafe(clickAction)
    );
}

#pragma endregion LIFECYCLE

#pragma region INPUT_CALLBACKS

void ADispatchPlayerController::HandleNextCamera()
{
    if (cameraManagerComponent)
    {
        cameraManagerComponent->CycleCameraRight();
    }
}

void ADispatchPlayerController::HandlePrevCamera()
{
    if (cameraManagerComponent)
    {
        cameraManagerComponent->CycleCameraLeft();
    }
}

void ADispatchPlayerController::HandleZoomPressed()
{
    if (cameraManagerComponent)
    {
        cameraManagerComponent->SetWantsZoom(true);
    }
}

void ADispatchPlayerController::HandleZoomReleased()
{
    if (cameraManagerComponent)
    {
        cameraManagerComponent->SetWantsZoom(false);
    }
}

void ADispatchPlayerController::HandleClick()
{
    if (cursorComponent)
    {
        cursorComponent->HandleClick();
    }
}


#pragma region INTERNAL

void ADispatchPlayerController::ForceApplyInitialDispatchCamera()
{
    if (!cameraManagerComponent)
    {
        return;
    }

    const int32 ActiveIdx = cameraManagerComponent->GetActiveCameraIndex();
    if (ActiveIdx != INDEX_NONE)
    {
        cameraManagerComponent->ActivateCameraByIndex(ActiveIdx);
    }
    else
    {
        cameraManagerComponent->ActivateCameraByIndex(0);
    }
}

#pragma endregion INTERNAL

#pragma endregion INPUT_CALLBACKS
