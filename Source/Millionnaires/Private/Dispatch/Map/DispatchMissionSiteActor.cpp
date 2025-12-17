/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionSiteActor" - Source
 * Notes: Represents a building/zone group that can be targeted by missions and highlighted via Overlay Material.
 */
#include "Dispatch/Map/DispatchMissionSiteActor.h"

#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

ADispatchMissionSiteActor::ADispatchMissionSiteActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ADispatchMissionSiteActor::BeginPlay()
{
    Super::BeginPlay();

    CacheMeshes();
    RestoreOriginalOverlays();

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchSite] BeginPlay | %s | Meshes=%d | Overlay=%s"),
            *GetName(), cachedMeshes.Num(), *GetNameSafe(highlightOverlayMaterial));
    }
}

void ADispatchMissionSiteActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bSmoothFade)
    {
        return;
    }

    if (!highlightOverlayMaterial || cachedMeshes.Num() == 0)
    {
        return;
    }

    const float Speed = (targetAlpha > currentAlpha) ? fadeInSpeed : fadeOutSpeed;
    currentAlpha = FMath::FInterpConstantTo(currentAlpha, targetAlpha, DeltaSeconds, FMath::Max(0.01f, Speed));

    ApplyInstant(currentAlpha);
}

#pragma region API

void ADispatchMissionSiteActor::SetVisualState(EDispatchMissionSiteVisualState NewState)
{
    if (visualState == NewState)
    {
        return;
    }

    if (cachedMeshes.Num() == 0)
    {
        CacheMeshes();
    }

    visualState = NewState;
    targetAlpha = GetTargetAlphaForState(visualState);

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchSite] SetVisualState | %s | State=%d | TargetAlpha=%.2f | Meshes=%d"),
            *GetName(), (int32)visualState, targetAlpha, cachedMeshes.Num());
    }

    // IMPORTANT: Apply instantly at least once, so it works even if tick is disabled.
    if (!bSmoothFade)
    {
        currentAlpha = targetAlpha;
        ApplyInstant(currentAlpha);
    }
    else
    {
        ApplyInstant(FMath::Max(currentAlpha, (targetAlpha > 0.f ? 0.02f : 0.f)));
    }
}

void ADispatchMissionSiteActor::SetOfferActive(bool bActive)
{
    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchSite] SetOfferActive | %s | Active=%s"), *GetName(), bActive ? TEXT("true") : TEXT("false"));
    }

    SetVisualState(bActive ? EDispatchMissionSiteVisualState::Offer : EDispatchMissionSiteVisualState::None);
}

void ADispatchMissionSiteActor::SetMissionState(EDispatchMissionState NewMissionState)
{
    switch (NewMissionState)
    {
        case EDispatchMissionState::Accepted:
        case EDispatchMissionState::Traveling:
        case EDispatchMissionState::Working:
        case EDispatchMissionState::Returning:
            SetVisualState(EDispatchMissionSiteVisualState::Running);
            break;

        case EDispatchMissionState::Completed:
            SetVisualState(EDispatchMissionSiteVisualState::Completed);
            break;

        case EDispatchMissionState::Failed:
        case EDispatchMissionState::Aborted:
            SetVisualState(EDispatchMissionSiteVisualState::Failed);
            break;

        default:
            break;
    }
}

void ADispatchMissionSiteActor::RebuildMeshCache()
{
    CacheMeshes();

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchSite] RebuildMeshCache | %s | Meshes=%d"), *GetName(), cachedMeshes.Num());
    }
}

#pragma endregion API

#pragma region INTERNAL

void ADispatchMissionSiteActor::CacheMeshes()
{
    cachedMeshes.Reset();
    originalOverlayByMesh.Reset();

    auto GatherFromActor = [&](AActor* Actor)
    {
        if (!Actor) return;

        TArray<UMeshComponent*> Meshes;
        Actor->GetComponents<UMeshComponent>(Meshes, /*bIncludeFromChildActors=*/true);

        for (UMeshComponent* M : Meshes)
        {
            if (M)
            {
                cachedMeshes.Add(M);
            }
        }
    };

    if (bIncludeSelfMeshes)
    {
        GatherFromActor(this);
    }

    for (AActor* A : targetActors)
    {
        GatherFromActor(A);
    }

    originalOverlayByMesh.Reserve(cachedMeshes.Num());
    for (UMeshComponent* M : cachedMeshes)
    {
        originalOverlayByMesh.Add(M ? M->GetOverlayMaterial() : nullptr);
    }

    if (bDebugLogs)
    {
        UE_LOG(LogTemp, Log, TEXT("[DispatchSite] CacheMeshes | %s | Targets=%d | Meshes=%d"),
            *GetName(), targetActors.Num(), cachedMeshes.Num());
    }
}

void ADispatchMissionSiteActor::ApplyOverlayToMeshes(UMaterialInterface* Overlay)
{
    for (UMeshComponent* M : cachedMeshes)
    {
        if (M)
        {
            M->SetOverlayMaterial(Overlay);
        }
    }
}

void ADispatchMissionSiteActor::RestoreOriginalOverlays()
{
    for (int32 i = 0; i < cachedMeshes.Num(); ++i)
    {
        UMeshComponent* M = cachedMeshes[i];
        if (!M) continue;

        UMaterialInterface* Original = originalOverlayByMesh.IsValidIndex(i) ? originalOverlayByMesh[i] : nullptr;
        M->SetOverlayMaterial(Original);
    }
}

void ADispatchMissionSiteActor::EnsureOverlayMID()
{
    if (overlayMID || !highlightOverlayMaterial)
    {
        return;
    }

    overlayMID = UMaterialInstanceDynamic::Create(highlightOverlayMaterial, this);
}

float ADispatchMissionSiteActor::GetTargetAlphaForState(EDispatchMissionSiteVisualState State) const
{
    switch (State)
    {
        case EDispatchMissionSiteVisualState::Offer: return 1.f;
        case EDispatchMissionSiteVisualState::Running: return 1.f;
        case EDispatchMissionSiteVisualState::Failed: return 1.f;
        case EDispatchMissionSiteVisualState::Completed: return 1.f;
        default: return 0.f;
    }
}

void ADispatchMissionSiteActor::ApplyInstant(float Alpha)
{
    if (!highlightOverlayMaterial || cachedMeshes.Num() == 0)
    {
        return;
    }

    if (Alpha <= 0.f)
    {
        RestoreOriginalOverlays();
        return;
    }

    EnsureOverlayMID();
    if (overlayMID)
    {
        overlayMID->SetScalarParameterValue(highlightAlphaParamName, Alpha);
        ApplyOverlayToMeshes(overlayMID);
    }
    else
    {
        ApplyOverlayToMeshes(highlightOverlayMaterial);
    }
}

#pragma endregion INTERNAL
