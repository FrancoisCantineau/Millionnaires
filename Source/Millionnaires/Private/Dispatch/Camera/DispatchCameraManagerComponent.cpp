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
#include "Materials/MaterialInstanceDynamic.h"

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

    // Apply a startup camera (by default, prefer a non-Map camera).
    if (CameraSpots.Num() > 0)
    {
        const int32 StartupIdx = FindStartupCameraIndex();
        if (StartupIdx != INDEX_NONE)
        {
            ActivateCameraByIndex(StartupIdx);
        }
    }
}

void UDispatchCameraManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeltaTime <= 0.f)
    {
        return;
    }
    
    if (!bSuspended)
    {
        UpdateMouseParallax(DeltaTime);
    }

    UpdateZoom(DeltaTime);
    UpdatePostProcessTransition(DeltaTime);
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
        const int32 StartupIdx = FindStartupCameraIndex();
        if (StartupIdx != INDEX_NONE)
        {
            ActivateCameraByIndex(StartupIdx);
        }
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

int32 UDispatchCameraManagerComponent::FindMapCameraIndex() const
{
    const int32 Num = CameraSpots.Num();
    for (int32 i = 0; i < Num; ++i)
    {
        const ADispatchCameraSpot* Spot = CameraSpots[i].Get();
        if (Spot && Spot->IsMapCamera())
        {
            return i;
        }
    }
    return INDEX_NONE;
}

int32 UDispatchCameraManagerComponent::FindStartupCameraIndex() const
{
    if (CameraSpots.Num() <= 0)
    {
        return INDEX_NONE;
    }

    if (!bPreferNonMapAtStartup)
    {
        return 0;
    }

    const ADispatchCameraSpot* First = CameraSpots[0].Get();
    if (First && First->IsMapCamera())
    {
        const int32 Next = FindNextCyclableCameraIndex(0, +1);
        if (Next != INDEX_NONE)
        {
            return Next;
        }
    }

    return 0;
}

int32 UDispatchCameraManagerComponent::FindNextCyclableCameraIndex(int32 FromIndex, int32 Direction) const
{
    const int32 Num = CameraSpots.Num();
    if (Num <= 0)
    {
        return INDEX_NONE;
    }

    const int32 Dir = (Direction >= 0) ? 1 : -1;

    // Try all cameras at most once
    for (int32 Step = 1; Step <= Num; ++Step)
    {
        const int32 Candidate = (FromIndex + (Dir * Step) + Num) % Num;
        const ADispatchCameraSpot* Spot = CameraSpots[Candidate].Get();
        if (!Spot)
        {
            continue;
        }

        // Prevent entering the Map camera via slide
        if (Spot->IsMapCamera())
        {
            continue;
        }

        return Candidate;
    }

    return INDEX_NONE;
}

void UDispatchCameraManagerComponent::CycleCameraRight()
{
    SortCameraSpots();
    
    if (CameraSpots.Num() == 0)
    {
        return;
    }

    int32 FromIndex = ActiveCameraIndex;
    if (!CameraSpots.IsValidIndex(FromIndex))
    {
        FromIndex = 0;
    }

    const int32 NewIndex = FindNextCyclableCameraIndex(FromIndex, +1);
    if (NewIndex == INDEX_NONE)
    {
        return;
    }

    if (DefaultTransitionPostProcessMaterial && DefaultTransitionPostProcessDuration > 0.f)
    {
        ActivateCameraByIndexWithPostProcessTransition(NewIndex, DefaultTransitionPostProcessMaterial, DefaultTransitionPostProcessDuration, DefaultTransitionPostProcessWeight);
    }
    else
    {
        ActivateCameraByIndex(NewIndex);
    }
}

void UDispatchCameraManagerComponent::CycleCameraLeft()
{
    SortCameraSpots();
    
    if (CameraSpots.Num() == 0)
    {
        return;
    }

    int32 FromIndex = ActiveCameraIndex;
    if (!CameraSpots.IsValidIndex(FromIndex))
    {
        FromIndex = 0;
    }

    const int32 NewIndex = FindNextCyclableCameraIndex(FromIndex, -1);
    if (NewIndex == INDEX_NONE)
    {
        return;
    }
    
    if (DefaultTransitionPostProcessMaterial && DefaultTransitionPostProcessDuration > 0.f)
    {
        ActivateCameraByIndexWithPostProcessTransition(NewIndex, DefaultTransitionPostProcessMaterial, DefaultTransitionPostProcessDuration, DefaultTransitionPostProcessWeight);
    }
    else
    {
        ActivateCameraByIndex(NewIndex);
    }
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
    const bool bIsMapCamera = NewCamera->IsMapCamera();

    if (bSuspended && !bIsMapCamera)
    {
        SetSuspended(false);
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    const float BlendTime = FMath::Max(NewCamera->GetBlendTime(), 0.f);
    ResetMouseParallax(OldCamera);
    PC->SetViewTargetWithBlend(NewCamera, BlendTime);

    ActiveCameraIndex = Index;
    ResetMouseParallax(NewCamera);

    // Reset virtual cursor (keeps parallax stable if the real cursor is captured/hidden).
    VirtualCursorX01 = 0.5f;
    VirtualCursorY01 = 0.5f;

    // Reset zoom when switching cameras.
    bWantsZoom = false;
    CurrentZoomAlpha = 0.f;

    OnActiveCameraChanged.Broadcast(NewCamera);
}

bool UDispatchCameraManagerComponent::ActivateMapCamera(bool bUseTransition)
{
    SortCameraSpots();

    const int32 MapIndex = FindMapCameraIndex();
    if (MapIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DispatchCamera] ActivateMapCamera failed: no camera has bIsMapCamera=true."));
        return false;
    }

    // Suspend camera updates while on map.
    SetSuspended(true);

    if (bUseTransition)
    {
        ActivateCameraByIndexWithPostProcessTransition(MapIndex, nullptr, 0.f, 0.f);
    }
    else
    {
        ActivateCameraByIndex(MapIndex);
    }

    return true;
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

void UDispatchCameraManagerComponent::ActivateCameraByIndexWithPostProcessTransition(int32 Index, UMaterialInterface* PostProcessMaterial, float Duration, float Weight)
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

    ADispatchCameraSpot* OldCamera = GetActiveCamera();

    UMaterialInterface* FinalMaterial = PostProcessMaterial ? PostProcessMaterial : DefaultTransitionPostProcessMaterial.Get();
    const float FinalDuration = (Duration > 0.f) ? Duration : DefaultTransitionPostProcessDuration;
    const float FinalWeight = (Weight > 0.f) ? Weight : DefaultTransitionPostProcessWeight;

    if (!FinalMaterial || FinalDuration <= 0.f)
    {
        ActivateCameraByIndex(Index);
        return;
    }

    StopPostProcessTransition();

    ActiveTransitionPP = UMaterialInstanceDynamic::Create(FinalMaterial, this);
    ActiveTransitionPPRemaining = FinalDuration;
    ActiveTransitionPPWeight = FinalWeight;
    bTransitionSkipFirstTick = true;

    auto EnsureOnCamera = [this](UCameraComponent* CameraComp)
    {
        if (!CameraComp || !ActiveTransitionPP) return;

        // Backup once
        FDispatchCameraPPBackup* BackupPtr = nullptr;
        for (FDispatchCameraPPBackup& B : ActiveTransitionPPBackups)
        {
            if (B.Camera.Get() == CameraComp)
            {
                BackupPtr = &B;
                break;
            }
        }

        if (!BackupPtr)
        {
            FDispatchCameraPPBackup& B = ActiveTransitionPPBackups.AddDefaulted_GetRef();
            B.Camera = CameraComp;
            B.SavedPostProcessBlendWeight = CameraComp->PostProcessBlendWeight;
            B.SavedBlendables = CameraComp->PostProcessSettings.WeightedBlendables;
            BackupPtr = &B;
        }

        // Force camera PP to be active
        CameraComp->PostProcessBlendWeight = 1.f;

        // Inject into the SAME container as the editor ("Post Process Materials"/Blendables)
        TArray<FWeightedBlendable>& Arr = CameraComp->PostProcessSettings.WeightedBlendables.Array;

        int32 Found = INDEX_NONE;
        for (int32 i = 0; i < Arr.Num(); ++i)
        {
            if (Arr[i].Object == ActiveTransitionPP)
            {
                Found = i;
                break;
            }
        }

        if (Found == INDEX_NONE)
        {
            FWeightedBlendable WB;
            WB.Object = ActiveTransitionPP;
            WB.Weight = ActiveTransitionPPWeight;
            Arr.Add(WB);
        }
        else
        {
            Arr[Found].Weight = ActiveTransitionPPWeight;
        }

        CameraComp->MarkRenderStateDirty();
    };

    if (OldCamera) EnsureOnCamera(OldCamera->GetCameraComponent());
    EnsureOnCamera(NewCamera->GetCameraComponent());

    ActivateCameraByIndex(Index);
}


void UDispatchCameraManagerComponent::PlayPostProcessTransition(UMaterialInterface* PostProcessMaterial, float Duration, float Weight)
{
    StopPostProcessTransition();

    UMaterialInterface* FinalMaterial = PostProcessMaterial ? PostProcessMaterial : DefaultTransitionPostProcessMaterial.Get();
    const float FinalDuration = (Duration > 0.f) ? Duration : DefaultTransitionPostProcessDuration;
    const float FinalWeight = (Weight > 0.f) ? Weight : DefaultTransitionPostProcessWeight;

    if (!FinalMaterial || FinalDuration <= 0.f)
    {
        return;
    }

    ActiveTransitionPP = UMaterialInstanceDynamic::Create(FinalMaterial, this);
    ActiveTransitionPPRemaining = FinalDuration;
    ActiveTransitionPPWeight = FinalWeight;
    bTransitionSkipFirstTick = true;

    ADispatchCameraSpot* Active = GetActiveCamera();
    if (!Active) return;

    if (UCameraComponent* CameraComp = Active->GetCameraComponent())
    {
        // Backup
        FDispatchCameraPPBackup& B = ActiveTransitionPPBackups.AddDefaulted_GetRef();
        B.Camera = CameraComp;
        B.SavedPostProcessBlendWeight = CameraComp->PostProcessBlendWeight;
        B.SavedBlendables = CameraComp->PostProcessSettings.WeightedBlendables;

        // Apply
        CameraComp->PostProcessBlendWeight = 1.f;
        FWeightedBlendable WB;
        WB.Object = ActiveTransitionPP;
        WB.Weight = ActiveTransitionPPWeight;
        CameraComp->PostProcessSettings.WeightedBlendables.Array.Add(WB);

        CameraComp->MarkRenderStateDirty();
    }
}

void UDispatchCameraManagerComponent::StopPostProcessTransition()
{
    for (const FDispatchCameraPPBackup& B : ActiveTransitionPPBackups)
    {
        if (UCameraComponent* CameraComp = B.Camera.Get())
        {
            CameraComp->PostProcessBlendWeight = B.SavedPostProcessBlendWeight;
            CameraComp->PostProcessSettings.WeightedBlendables = B.SavedBlendables;

            CameraComp->MarkRenderStateDirty();
        }
    }

    ActiveTransitionPPBackups.Reset();
    ActiveTransitionPP = nullptr;
    ActiveTransitionPPRemaining = 0.f;
    ActiveTransitionPPWeight = 1.f;
    bTransitionSkipFirstTick = false;
}

void UDispatchCameraManagerComponent::UpdatePostProcessTransition(float DeltaTime)
{
    if (!ActiveTransitionPP || ActiveTransitionPPRemaining <= 0.f)
    {
        return;
    }

    // Guarantees at least one rendered frame.
    if (bTransitionSkipFirstTick)
    {
        bTransitionSkipFirstTick = false;
        return;
    }

    // Re-ensure each tick (in case UpdateZoom or other logic rewrites PostProcessSettings)
    for (const FDispatchCameraPPBackup& B : ActiveTransitionPPBackups)
    {
        if (UCameraComponent* CameraComp = B.Camera.Get())
        {
            CameraComp->PostProcessBlendWeight = 1.f;

            TArray<FWeightedBlendable>& Arr = CameraComp->PostProcessSettings.WeightedBlendables.Array;
            bool bFound = false;

            for (FWeightedBlendable& WB : Arr)
            {
                if (WB.Object == ActiveTransitionPP)
                {
                    WB.Weight = ActiveTransitionPPWeight;
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                FWeightedBlendable WB;
                WB.Object = ActiveTransitionPP;
                WB.Weight = ActiveTransitionPPWeight;
                Arr.Add(WB);
            }
        }
    }

    ActiveTransitionPPRemaining -= DeltaTime;

    if (ActiveTransitionPPRemaining <= 0.f)
    {
        StopPostProcessTransition();
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

    // Keep parallax stable when the real cursor becomes captured/hidden.
    VirtualCursorX01 = 0.5f;
    VirtualCursorY01 = 0.5f;

    // Ensure the Dispatch cursor remains usable when switching in/out of Map camera.
    if (APlayerController* PC = GetOwningPlayerController())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
        PC->SetInputMode(InputMode);

        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }

    if (bSuspended)
    {
        // Stop zoom while map is open.
        bWantsZoom = false;
    }
}


static float ApplyDeadZone01(float V, float DeadZone)
{
    const float A = FMath::Abs(V);
    if (A <= DeadZone)
    {
        return 0.f;
    }

    const float Sign = FMath::Sign(V);
    const float Scaled = (A - DeadZone) / FMath::Max(1.f - DeadZone, KINDA_SMALL_NUMBER);
    return Sign * FMath::Clamp(Scaled, 0.f, 1.f);
}

void UDispatchCameraManagerComponent::ResetMouseParallax(ADispatchCameraSpot* CameraSpot)
{
    if (!CameraSpot) return;

    UCameraComponent* Cam = CameraSpot->GetCameraComponent();
    if (!Cam) return;

    Cam->SetRelativeRotation(CameraSpot->GetInitialCameraRelativeRotation());
    Cam->SetRelativeLocation(CameraSpot->GetInitialCameraRelativeLocation());
}

void UDispatchCameraManagerComponent::UpdateMouseParallax(float DeltaTime)
{
    ADispatchCameraSpot* Active = GetActiveCamera();
    if (!Active || !Active->UsesMouseParallax())
    {
        return;
    }

    UCameraComponent* Cam = Active->GetCameraComponent();
    if (!Cam)
    {
        return;
    }

    APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        return;
    }

    int32 SizeX = 0;
    int32 SizeY = 0;
    PC->GetViewportSize(SizeX, SizeY);
    if (SizeX <= 0 || SizeY <= 0)
    {
        return;
    }

    float MouseX = 0.f;
    float MouseY = 0.f;
    const bool bHasMousePos = PC->GetMousePosition(MouseX, MouseY);

    // If the mouse is captured/hidden, GetMousePosition may fail (or stay stuck).
    // In that case, fall back to deltas and maintain a virtual cursor in [0..1].
    if (bHasMousePos && PC->bShowMouseCursor)
    {
        VirtualCursorX01 = FMath::Clamp(MouseX / (float)SizeX, 0.f, 1.f);
        VirtualCursorY01 = FMath::Clamp(MouseY / (float)SizeY, 0.f, 1.f);
    }
    else
    {
        float DeltaX = 0.f;
        float DeltaY = 0.f;
        PC->GetInputMouseDelta(DeltaX, DeltaY);

        VirtualCursorX01 = FMath::Clamp(VirtualCursorX01 + (DeltaX / (float)SizeX), 0.f, 1.f);
        VirtualCursorY01 = FMath::Clamp(VirtualCursorY01 + (DeltaY / (float)SizeY), 0.f, 1.f);
    }

    // Normalize mouse position to [-1..1] with (0,0) center.
    float NX = (VirtualCursorX01 - 0.5f) * 2.f;
    float NY = (VirtualCursorY01 - 0.5f) * 2.f;

    // Deadzone to avoid micro jitter.
    const float DZ = Active->GetMouseParallaxDeadZone();
    NX = ApplyDeadZone01(NX, DZ);
    NY = ApplyDeadZone01(NY, DZ);

    // Compute target offsets.
    const float TargetYaw = NX * Active->GetMouseParallaxMaxYaw();

    float TargetPitch = -NY * Active->GetMouseParallaxMaxPitch();
    if (Active->IsMouseParallaxYInverted())
    {
        TargetPitch *= -1.f;
    }

    const FVector MaxLoc = Active->GetMouseParallaxMaxLocationOffset();
    const FVector TargetLocOffset = FVector(
        MaxLoc.X * 0.f,            // generally keep X at 0
        NX * MaxLoc.Y,
        -NY * MaxLoc.Z
    );

    const FRotator BaseRot = Active->GetInitialCameraRelativeRotation();
    const FVector BaseLoc = Active->GetInitialCameraRelativeLocation();

    const FRotator DesiredRot = BaseRot + FRotator(TargetPitch, TargetYaw, 0.f);
    const FVector DesiredLoc = BaseLoc + TargetLocOffset;

    const float Speed = Active->GetMouseParallaxInterpSpeed();

    Cam->SetRelativeRotation(FMath::RInterpTo(Cam->GetRelativeRotation(), DesiredRot, DeltaTime, Speed));
    Cam->SetRelativeLocation(FMath::VInterpTo(Cam->GetRelativeLocation(), DesiredLoc, DeltaTime, Speed));
}
