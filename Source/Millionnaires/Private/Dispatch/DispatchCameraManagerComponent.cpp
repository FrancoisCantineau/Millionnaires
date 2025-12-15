/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCameraManagerComponent" - Source
 * Notes: Implementation of the Dispatch camera manager component.
 */

#include "Dispatch/DispatchCameraManagerComponent.h"

#include "Dispatch/DispatchCameraSpot.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"

UDispatchCameraManagerComponent::UDispatchCameraManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    ActiveCameraIndex = INDEX_NONE;
    bWantsZoom = false;
    CurrentZoomAlpha = 0.f;
    ActiveCameraInitialFOV = 90.f;

    bHasBaseRotation = false;
    bHasZoomTarget = false;
}

void UDispatchCameraManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoDiscoverCameras && GetWorld())
    {
        for (TActorIterator<ADispatchCameraSpot> It(GetWorld()); It; ++It)
        {
            RegisterCameraSpot(*It);
        }
    }

    SortCameraSpots();

    // Optionally activate the first camera if none is active.
    if (ActiveCameraIndex == INDEX_NONE && CameraSpots.Num() > 0)
    {
        ActivateCameraByIndex(0);
    }
}

void UDispatchCameraManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeltaTime <= 0.f)
    {
        return;
    }

    // Zoom updates CurrentZoomAlpha (and may disable wants-zoom if the camera disallows it).
    UpdateZoom(DeltaTime);

    // Rotation behaviour depends on the zoom state & cursor position.
    UpdateActiveCameraRotation(DeltaTime);
}

#pragma region REGISTRATION

void UDispatchCameraManagerComponent::RegisterCameraSpot(ADispatchCameraSpot* CameraSpot)
{
    if (!CameraSpot)
    {
        return;
    }

    CameraSpots.AddUnique(CameraSpot);
    SortCameraSpots();

    if (ActiveCameraIndex == INDEX_NONE && CameraSpots.Num() > 0)
    {
        ActivateCameraByIndex(0);
    }
}

void UDispatchCameraManagerComponent::UnregisterCameraSpot(ADispatchCameraSpot* CameraSpot)
{
    if (!CameraSpot)
    {
        return;
    }

    CameraSpots.Remove(CameraSpot);

    if (CameraSpots.Num() == 0)
    {
        ActiveCameraIndex = INDEX_NONE;
    }
    else if (!CameraSpots.IsValidIndex(ActiveCameraIndex))
    {
        ActiveCameraIndex = 0;
        ActivateCameraByIndex(ActiveCameraIndex);
    }
}

#pragma endregion REGISTRATION

#pragma region CAMERA_CONTROL

void UDispatchCameraManagerComponent::CycleCameraRight()
{
    if (CameraSpots.Num() == 0)
    {
        return;
    }

    int32 NewIndex = ActiveCameraIndex;

    if (!CameraSpots.IsValidIndex(NewIndex))
    {
        NewIndex = 0;
    }

    NewIndex = (NewIndex + 1) % CameraSpots.Num();
    ActivateCameraByIndex(NewIndex);
}

void UDispatchCameraManagerComponent::CycleCameraLeft()
{
    if (CameraSpots.Num() == 0)
    {
        return;
    }

    int32 NewIndex = ActiveCameraIndex;

    if (!CameraSpots.IsValidIndex(NewIndex))
    {
        NewIndex = 0;
    }

    NewIndex = (NewIndex - 1 + CameraSpots.Num()) % CameraSpots.Num();
    ActivateCameraByIndex(NewIndex);
}

void UDispatchCameraManagerComponent::ActivateCameraByIndex(int32 Index)
{
    SortCameraSpots();

    if (!CameraSpots.IsValidIndex(Index))
    {
        return;
    }

    ADispatchCameraSpot* NewCamera = CameraSpots[Index].Get();
    if (!NewCamera)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    const float BlendTime = FMath::Max(NewCamera->GetBlendTime(), 0.f);
    PC->SetViewTargetWithBlend(NewCamera, BlendTime);

    ActiveCameraIndex = Index;

    // Reset zoom and behaviour state when switching cameras.
    bWantsZoom = false;
    CurrentZoomAlpha = 0.f;
    bHasZoomTarget = false;

    // Cache base rotation & base FOV for stable behaviour.
    BaseCameraRotation = NewCamera->GetActorRotation();
    bHasBaseRotation = true;

    if (UCameraComponent* CamComp = NewCamera->GetCameraComponent())
    {
        ActiveCameraInitialFOV = CamComp->FieldOfView;
    }
    else
    {
        ActiveCameraInitialFOV = 90.f;
    }

    OnActiveCameraChanged.Broadcast(NewCamera);
}

ADispatchCameraSpot* UDispatchCameraManagerComponent::GetActiveCamera() const
{
    if (!CameraSpots.IsValidIndex(ActiveCameraIndex))
    {
        return nullptr;
    }

    return CameraSpots[ActiveCameraIndex].Get();
}

#pragma endregion CAMERA_CONTROL

#pragma region ZOOM

void UDispatchCameraManagerComponent::SetWantsZoom(bool bInWantsZoom)
{
    if (bWantsZoom == bInWantsZoom)
    {
        return;
    }

    bWantsZoom = bInWantsZoom;

    if (bWantsZoom)
    {
        ComputeZoomTargetUnderCursor();
    }
}

void UDispatchCameraManagerComponent::ComputeZoomTargetUnderCursor()
{
    bHasZoomTarget = false;

    ADispatchCameraSpot* ActiveCamera = GetActiveCamera();
    if (!ActiveCamera)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    FVector WorldOrigin;
    FVector WorldDirection;
    if (!PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
    {
        return;
    }

    // Intersect a horizontal plane at the camera height (feels natural in a top-ish control room).
    const float PlaneZ = ActiveCamera->GetActorLocation().Z;
    const float Denominator = WorldDirection.Z;

    if (FMath::Abs(Denominator) < KINDA_SMALL_NUMBER)
    {
        return;
    }

    const float T = (PlaneZ - WorldOrigin.Z) / Denominator;
    if (T <= 0.f)
    {
        return;
    }

    ZoomTargetWorldLocation = WorldOrigin + WorldDirection * T;
    bHasZoomTarget = true;
}

void UDispatchCameraManagerComponent::UpdateZoom(float DeltaTime)
{
    ADispatchCameraSpot* ActiveCamera = GetActiveCamera();
    if (!ActiveCamera)
    {
        return;
    }

    UCameraComponent* CameraComp = ActiveCamera->GetCameraComponent();
    if (!CameraComp)
    {
        return;
    }

    if (!ActiveCamera->IsZoomAllowed())
    {
        bWantsZoom = false;
    }

    const float TargetAlpha = bWantsZoom ? 1.f : 0.f;
    const float InterpSpeed = bWantsZoom ? ActiveCamera->GetZoomInterpSpeedIn() : ActiveCamera->GetZoomInterpSpeedOut();

    CurrentZoomAlpha = FMath::FInterpTo(CurrentZoomAlpha, TargetAlpha, DeltaTime, InterpSpeed);

    // Use the cached initial FOV from activation (prevents BeginPlay order issues).
    const float NewFOV = FMath::Lerp(ActiveCameraInitialFOV, ActiveCamera->GetZoomedFOV(), CurrentZoomAlpha);
    CameraComp->SetFieldOfView(NewFOV);

    if (ActiveCamera->UsesZoomPostProcess())
    {
        CameraComp->PostProcessSettings = ActiveCamera->GetZoomPostProcessSettings();
        CameraComp->PostProcessBlendWeight = CurrentZoomAlpha * ActiveCamera->GetZoomPostProcessBlendWeight();
    }
    else
    {
        CameraComp->PostProcessBlendWeight = 0.f;
    }
}

#pragma endregion ZOOM

#pragma region CAMERA_BEHAVIOUR

void UDispatchCameraManagerComponent::UpdateActiveCameraRotation(float DeltaTime)
{
    ADispatchCameraSpot* ActiveCamera = GetActiveCamera();
    if (!ActiveCamera)
    {
        return;
    }

    if (!bHasBaseRotation)
    {
        BaseCameraRotation = ActiveCamera->GetActorRotation();
        bHasBaseRotation = true;
    }

    // If all behaviours are disabled, do nothing.
    const bool bAnyMouse = bEnableMouseParallax;
    const bool bAnyLookAt = bEnableZoomLookAt;

    if (!bAnyMouse && !bAnyLookAt)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    FVector2D ViewportSize = FVector2D::ZeroVector;
    if (UGameViewportClient* ViewportClient = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
    {
        ViewportClient->GetViewportSize(ViewportSize);
    }

    FVector2D MousePos = FVector2D::ZeroVector;
    if (!PC->GetMousePosition(MousePos.X, MousePos.Y) || ViewportSize.X <= 0.f || ViewportSize.Y <= 0.f)
    {
        MousePos = ViewportSize * 0.5f;
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

    if (bEnableMouseParallax)
    {
        TargetRot.Yaw += OffsetNDC.X * MouseYawAmplitude;
        TargetRot.Pitch -= OffsetNDC.Y * MousePitchAmplitude;
    }

    // If zooming and we have a valid world target, bias the rotation towards it.
    if (bEnableZoomLookAt && CurrentZoomAlpha > 0.01f && bHasZoomTarget)
    {
        const FVector From = ActiveCamera->GetActorLocation();
        const FVector To = ZoomTargetWorldLocation;

        const FRotator LookAtRot = (To - From).Rotation();
        TargetRot = FMath::RInterpTo(TargetRot, LookAtRot, DeltaTime, ZoomLookAtInterpSpeed);
    }

    const FRotator NewRot = FMath::RInterpTo(
        ActiveCamera->GetActorRotation(),
        TargetRot,
        DeltaTime,
        CameraRotationInterpSpeed
    );

    ActiveCamera->SetActorRotation(NewRot);
}

#pragma endregion CAMERA_BEHAVIOUR

#pragma region INTERNAL

void UDispatchCameraManagerComponent::SortCameraSpots()
{
    CameraSpots.RemoveAll([](const TWeakObjectPtr<ADispatchCameraSpot>& Spot)
    {
        return !Spot.IsValid();
    });

    CameraSpots.Sort([](const TWeakObjectPtr<ADispatchCameraSpot>& A, const TWeakObjectPtr<ADispatchCameraSpot>& B)
    {
        const ADispatchCameraSpot* SpotA = A.Get();
        const ADispatchCameraSpot* SpotB = B.Get();

        const int32 IndexA = SpotA ? SpotA->GetCameraIndex() : 0;
        const int32 IndexB = SpotB ? SpotB->GetCameraIndex() : 0;
        return IndexA < IndexB;
    });

    if (CameraSpots.Num() == 0)
    {
        ActiveCameraIndex = INDEX_NONE;
    }
    else if (!CameraSpots.IsValidIndex(ActiveCameraIndex))
    {
        ActiveCameraIndex = 0;
    }
}

APlayerController* UDispatchCameraManagerComponent::GetOwningPlayerController() const
{
    if (APlayerController* AsPC = Cast<APlayerController>(GetOwner()))
    {
        return AsPC;
    }

    if (GetWorld())
    {
        return GetWorld()->GetFirstPlayerController();
    }

    return nullptr;
}

#pragma endregion INTERNAL
