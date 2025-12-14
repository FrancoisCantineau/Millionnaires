/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchPlayerController"
 * Notes: Implements camera behaviour, fade and cursor-driven slow-mo for the Dispatch scene.
 */

#include "Dispatch/DispatchPlayerController.h"

#include "Dispatch/DispatchCameraManagerComponent.h"
#include "Dispatch/DispatchCursorComponent.h"
#include "Dispatch/DispatchCursorRadialWidget.h"
#include "Dispatch/DispatchCameraSpot.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "TimerManager.h"

ADispatchPlayerController::ADispatchPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;

    // Create sub components
    CameraManagerComponent = CreateDefaultSubobject<UDispatchCameraManagerComponent>(TEXT("DispatchCameraManager"));
    CursorComponent = CreateDefaultSubobject<UDispatchCursorComponent>(TEXT("DispatchCursor"));

    // --- Default config values ---

    MouseYawAmplitude = 8.f;
    MousePitchAmplitude = 4.f;
    CameraRotationInterpSpeed = 5.f;

    ZoomedFOV = 30.f;
    ZoomFOVInterpSpeed = 5.f;
    ZoomLookAtInterpSpeed = 4.f;

    CameraFadeOutDuration = 0.35f;
    CameraFadeInDuration = 0.35f;

    bHasCameraDefaults = false;
    bWantsZoom = false;
    bHasZoomTarget = false;
    PendingCameraSwitch = EDispatchPendingCameraSwitch::None;
    bIsCameraFading = false;

    DefaultMouseCursor = EMouseCursor::Default;
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

    // Bind to camera manager delegate if available.
    if (CameraManagerComponent)
    {
        CameraManagerComponent->OnActiveCameraChanged.AddDynamic(this, &ADispatchPlayerController::HandleActiveCameraChanged);
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

    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent))
    {
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
            EI->BindAction(ZoomAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleZoomPressed);
            EI->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ADispatchPlayerController::HandleZoomReleased);
            EI->BindAction(ZoomAction, ETriggerEvent::Canceled, this, &ADispatchPlayerController::HandleZoomReleased);
        }

        if (ClickAction)
        {
            EI->BindAction(ClickAction, ETriggerEvent::Started, this, &ADispatchPlayerController::HandleCursorClick);
        }
    }
}

void ADispatchPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    UpdateCameraMouseAndZoom(DeltaTime);
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
    bWantsZoom = true;
    UE_LOG(LogTemp, Warning, TEXT("[Dispatch] Zoom pressed"));
    
    if (!CurrentCameraSpot.IsValid())
    {
        bHasZoomTarget = false;
        return;
    }

    FVector WorldOrigin;
    FVector WorldDirection;
    if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
    {
        bHasZoomTarget = false;
        return;
    }

    // Intersect a horizontal plane at the camera height.
    const float PlaneZ = CurrentCameraSpot->GetActorLocation().Z;
    const float Denominator = WorldDirection.Z;

    if (FMath::Abs(Denominator) < KINDA_SMALL_NUMBER)
    {
        bHasZoomTarget = false;
        return;
    }

    const float T = (PlaneZ - WorldOrigin.Z) / Denominator;
    if (T <= 0.f)
    {
        bHasZoomTarget = false;
        return;
    }

    ZoomTargetWorldLocation = WorldOrigin + WorldDirection * T;
    bHasZoomTarget = true;
}

void ADispatchPlayerController::HandleZoomReleased()
{
    bWantsZoom = false;
    // Keep the last zoom target so that releasing zoom smoothly interpolates back.
}

#pragma endregion CAMERA_API

#pragma region CURSOR_API

void ADispatchPlayerController::HandleCursorClick()
{
    if (!CursorComponent)
    {
        return;
    }

    AActor* HoveredActor = CursorComponent->GetCurrentHoveredActor();

    // Clicking on a hologram (tag-based) opens the map.
    if (HoveredActor && HoveredActor->ActorHasTag(FName(TEXT("DispatchMapHologram"))))
    {
        OpenMap();
        return;
    }

    // Fallback: let the cursor component handle generic interaction if needed.
    CursorComponent->HandleClick();
}

#pragma endregion CURSOR_API

#pragma region MAP_API

void ADispatchPlayerController::OpenMap()
{
    if (ActiveMapWidget || !MapWidgetClass)
    {
        return;
    }

    ActiveMapWidget = CreateWidget<UUserWidget>(this, MapWidgetClass);
    if (ActiveMapWidget)
    {
        ActiveMapWidget->AddToViewport(5);
        // Optionally lock input / capture the cursor here if desired.
        bShowMouseCursor = true;
    }
}

void ADispatchPlayerController::CloseMap()
{
    if (ActiveMapWidget)
    {
        ActiveMapWidget->RemoveFromParent();
        ActiveMapWidget = nullptr;
    }
}

void ADispatchPlayerController::ToggleMap()
{
    if (ActiveMapWidget)
    {
        CloseMap();
    }
    else
    {
        OpenMap();
    }
}

#pragma endregion MAP_API

#pragma region INTERNAL_CALLBACKS

void ADispatchPlayerController::HandleActiveCameraChanged(ADispatchCameraSpot* NewCamera)
{
    CurrentCameraSpot = NewCamera;
    bHasCameraDefaults = false;
    bHasZoomTarget = false;
    bWantsZoom = false;

    // Auto-close map whenever we change the Dispatch camera.
    CloseMap();

    if (!CurrentCameraSpot.IsValid())
    {
        return;
    }

    // Cache default FOV & rotation from the new camera.
    if (UCameraComponent* Cam = CurrentCameraSpot->FindComponentByClass<UCameraComponent>())
    {
        DefaultCameraFOV = Cam->FieldOfView;
        CurrentCameraFOV = DefaultCameraFOV;
    }

    BaseCameraRotation = CurrentCameraSpot->GetActorRotation();
    bHasCameraDefaults = true;
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

void ADispatchPlayerController::UpdateCameraMouseAndZoom(float DeltaTime)
{
    if (!CurrentCameraSpot.IsValid())
    {
        return;
    }

    UCameraComponent* CamComp = CurrentCameraSpot->FindComponentByClass<UCameraComponent>();
    if (!CamComp)
    {
        return;
    }

    if (!bHasCameraDefaults)
    {
        DefaultCameraFOV = CamComp->FieldOfView;
        CurrentCameraFOV = DefaultCameraFOV;
        BaseCameraRotation = CurrentCameraSpot->GetActorRotation();
        bHasCameraDefaults = true;
    }

    // --- Compute mouse offset in viewport space ---
    FVector2D ViewportSize = FVector2D::ZeroVector;
    if (UGameViewportClient* ViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
    {
        ViewportClient->GetViewportSize(ViewportSize);
    }

    FVector2D MousePos = FVector2D::ZeroVector;
    if (!GetMousePosition(MousePos.X, MousePos.Y) || ViewportSize.X <= 0.f || ViewportSize.Y <= 0.f)
    {
        // Even if we cannot read mouse, still smoothly restore default FOV/rotation.
        float TargetFOVNoMouse = bWantsZoom ? ZoomedFOV : DefaultCameraFOV;
        CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, TargetFOVNoMouse, DeltaTime, ZoomFOVInterpSpeed);
        CamComp->SetFieldOfView(CurrentCameraFOV);
        return;
    }

    const FVector2D Center = ViewportSize * 0.5f;
    FVector2D OffsetNDC(
        (MousePos.X - Center.X) / (ViewportSize.X * 0.5f),
        (MousePos.Y - Center.Y) / (ViewportSize.Y * 0.5f)
    );
    OffsetNDC.X = FMath::Clamp(OffsetNDC.X, -1.f, 1.f);
    OffsetNDC.Y = FMath::Clamp(OffsetNDC.Y, -1.f, 1.f);

    // Base rotation is the "rest" pose of the camera spot.
    FRotator TargetRot = BaseCameraRotation;
    TargetRot.Yaw += OffsetNDC.X * MouseYawAmplitude;
    TargetRot.Pitch -= OffsetNDC.Y * MousePitchAmplitude;

    // If zooming and we have a valid world target, bias the rotation towards it.
    if (bWantsZoom && bHasZoomTarget)
    {
        const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
            CurrentCameraSpot->GetActorLocation(),
            ZoomTargetWorldLocation
        );

        TargetRot = FMath::RInterpTo(TargetRot, LookAtRot, DeltaTime, ZoomLookAtInterpSpeed);
    }

    const FRotator NewRot = FMath::RInterpTo(
        CurrentCameraSpot->GetActorRotation(),
        TargetRot,
        DeltaTime,
        CameraRotationInterpSpeed
    );
    CurrentCameraSpot->SetActorRotation(NewRot);

    // --- FOV zoom ---
    const float TargetFOV = bWantsZoom ? ZoomedFOV : DefaultCameraFOV;
    CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, TargetFOV, DeltaTime, ZoomFOVInterpSpeed);
    CamComp->SetFieldOfView(CurrentCameraFOV);
}

#pragma endregion INTERNAL_CALLBACKS
