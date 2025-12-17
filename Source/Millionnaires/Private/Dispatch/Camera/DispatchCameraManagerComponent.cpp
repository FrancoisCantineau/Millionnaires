/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCameraManagerComponent" - Source
 * Notes: Implementation of the Dispatch camera manager component.
 */

#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Dispatch/Camera/DispatchCameraSpot.h"
#include "Dispatch/DispatchTypes.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Millionnaires.h"
#include "GameFramework/PlayerController.h"

UDispatchCameraManagerComponent::UDispatchCameraManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    ActiveCameraIndex = INDEX_NONE;
    bWantsZoom = false;
    CurrentZoomAlpha = 0.f;
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

    UpdateZoom(DeltaTime);
}

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

bool UDispatchCameraManagerComponent::IsOnMapCamera() const
{
    if (ADispatchCameraSpot* Active = GetActiveCamera())
    {
        return Active->IsMapCamera();
    }
    return false;
}

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

    // --- Check if we are leaving the map camera, automatically reset the suspended/map state.
    ADispatchCameraSpot* OldCamera = GetActiveCamera();
    const bool bWasMapCamera = OldCamera && OldCamera->IsMapCamera();
    const bool bIsMapCamera = NewCamera->IsMapCamera();

    if (bSuspended && bWasMapCamera && !bIsMapCamera)
    {
        SetSuspended(false);
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    const float BlendTime = FMath::Max(NewCamera->GetBlendTime(), 0.f);
    PC->SetViewTargetWithBlend(NewCamera, BlendTime);

    ActiveCameraIndex = Index;

    // Reset zoom when switching cameras.
    bWantsZoom = false;
    CurrentZoomAlpha = 0.f;

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

void UDispatchCameraManagerComponent::SetWantsZoom(bool bInWantsZoom)
{
    bWantsZoom = bInWantsZoom;
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

    const float NewFOV = FMath::Lerp(ActiveCamera->GetInitialFOV(), ActiveCamera->GetZoomedFOV(), CurrentZoomAlpha);
    CameraComp->SetFieldOfView(NewFOV);

    if (ActiveCamera->UsesZoomPostProcess())
    {
        CameraComp->PostProcessSettings = ActiveCamera->GetZoomPostProcessSettings();
        CameraComp->PostProcessBlendWeight = CurrentZoomAlpha * ActiveCamera->GetZoomPostProcessBlendWeight();
    }
    else
    {
        CameraComp->PostProcessBlendWeight = 1.f;
    }
}

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


void UDispatchCameraManagerComponent::SetSuspended(bool bInSuspended)
{
    bSuspended = bInSuspended;

    if (bSuspended)
    {
        // Stop zoom while map is open.
        bWantsZoom = false;
    }
}
