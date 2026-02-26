/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMissionSiteActor" - Source
 * Notes: Represents a mission site in the Dispatch map and supports highlight + click targets.
 */
#include "Dispatch/Map/DispatchMissionSiteActor.h"

#include "Components/PrimitiveComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"


namespace DispatchSiteHighlight
{
    static bool IsMissionActiveState(const EDispatchMissionState State)
    {
        return State == EDispatchMissionState::Accepted
            || State == EDispatchMissionState::Traveling
            || State == EDispatchMissionState::Working
            || State == EDispatchMissionState::Returning;
    }

    static bool IsMissionEndedState(const EDispatchMissionState State)
    {
        return State == EDispatchMissionState::Completed
            || State == EDispatchMissionState::Failed
            || State == EDispatchMissionState::Aborted;
    }
}

#pragma region LIFECYCLE

ADispatchMissionSiteActor::ADispatchMissionSiteActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

#pragma endregion LIFECYCLE

#pragma region API

void ADispatchMissionSiteActor::RebuildMeshCache()
{
    cachedPrimitives.Reset();
    cachedMeshes.Reset();

    // NOTE: we keep previousOverlayByMesh to restore the *original* overlay when highlight fully ends.
    // If you rebuild cache while highlighted, we should not lose previous overlays for already tracked meshes.
    if (bRestorePreviousOverlayMaterial)
    {
        previousOverlayByMesh.Reset();
    }

    // Collect primitives + meshes from self.
    {
        TArray<UPrimitiveComponent*> Prims;
        GetComponents(Prims);
        for (UPrimitiveComponent* P : Prims)
        {
            if (P)
            {
                cachedPrimitives.Add(P);
            }
        }

        TArray<UMeshComponent*> Meshes;
        GetComponents(Meshes);
        for (UMeshComponent* M : Meshes)
        {
            if (M)
            {
                cachedMeshes.Add(M);
            }
        }
    }

    // Collect primitives + meshes from targets.
    for (AActor* A : targetActors)
    {
        if (!A) continue;

        TArray<UPrimitiveComponent*> Prims;
        A->GetComponents(Prims);
        for (UPrimitiveComponent* P : Prims)
        {
            if (P)
            {
                cachedPrimitives.Add(P);
            }
        }

        TArray<UMeshComponent*> Meshes;
        A->GetComponents(Meshes);
        for (UMeshComponent* M : Meshes)
        {
            if (M)
            {
                cachedMeshes.Add(M);
            }
        }
    }

    RefreshHighlight();
}


void ADispatchMissionSiteActor::SetOfferActive(bool bActive)
{
    bOfferActive = bActive;

    // If an offer comes back, stop any expiry pulse.
    if (bActive)
    {
        bOfferExpiredPulseActive = false;
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(offerExpiredPulseTimerHandle);
        }
    }

    if (cachedPrimitives.Num() == 0 && cachedMeshes.Num() == 0)
    {
        RebuildMeshCache();
        return;
    }

    RefreshHighlight();
}


void ADispatchMissionSiteActor::SetMissionState(EDispatchMissionState NewState)
{
    currentMissionState = NewState;

    // When a mission is running, an offer shouldn't keep highlighting this place.
    if (DispatchSiteHighlight::IsMissionActiveState(NewState))
    {
        bOfferActive = false;

        // Also stop any expiry pulse.
        bOfferExpiredPulseActive = false;
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(offerExpiredPulseTimerHandle);
        }
    }

    if (cachedPrimitives.Num() == 0 && cachedMeshes.Num() == 0)
    {
        RebuildMeshCache();
        return;
    }

    RefreshHighlight();
}



void ADispatchMissionSiteActor::PlayOfferExpiredPulse()
{
    // If a mission is currently running here, we do not play the expired pulse.
    if (DispatchSiteHighlight::IsMissionActiveState(currentMissionState))
    {
        return;
    }

    // The offer is gone by definition.
    bOfferActive = false;

    if (cachedPrimitives.Num() == 0 && cachedMeshes.Num() == 0)
    {
        RebuildMeshCache();
    }

    bOfferExpiredPulseActive = true;
    RefreshHighlight();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(offerExpiredPulseTimerHandle);
        World->GetTimerManager().SetTimer(
            offerExpiredPulseTimerHandle,
            this,
            &ADispatchMissionSiteActor::EndOfferExpiredPulse,
            FMath::Max(0.01f, offerExpiredPulseDurationSec),
            false
        );
    }
}

void ADispatchMissionSiteActor::RefreshHighlight()
{
    const bool bMissionActive = DispatchSiteHighlight::IsMissionActiveState(currentMissionState);
    const bool bMissionEnded = DispatchSiteHighlight::IsMissionEndedState(currentMissionState);

    // Safety: if mission ended, we don't highlight unless we are mid-pulse (shouldn't happen).
    if (bMissionEnded && !bOfferExpiredPulseActive)
    {
        ApplyHighlight(false, customDepthStencilValue);
        return;
    }

    const bool bShouldHighlight = bOfferExpiredPulseActive || bMissionActive || bOfferActive;
    ApplyHighlight(bShouldHighlight, customDepthStencilValue);
}

void ADispatchMissionSiteActor::EndOfferExpiredPulse()
{
    bOfferExpiredPulseActive = false;
    RefreshHighlight();
}

#pragma endregion API

#pragma region INTERNAL

void ADispatchMissionSiteActor::ApplyHighlight(bool bEnabled, int32 Stencil)
{
    const bool bMissionActive = DispatchSiteHighlight::IsMissionActiveState(currentMissionState);

    // 1) OverlayMaterial highlight (preferred).
    if (bUseOverlayMaterialHighlight)
    {
        UMaterialInterface* DesiredOverlay = nullptr;

        // Priority: Expired pulse > Active mission > Offer.
        if (bEnabled && bOfferExpiredPulseActive)
        {
            DesiredOverlay = offerExpiredPulseOverlayMaterial;
        }
        if (bEnabled && !DesiredOverlay && bMissionActive)
        {
            DesiredOverlay = activeMissionOverlayMaterial ? activeMissionOverlayMaterial : overlayHighlightMaterial;
        }
        if (bEnabled && !DesiredOverlay && bOfferActive)
        {
            DesiredOverlay = overlayHighlightMaterial;
        }

        if (bEnabled && !DesiredOverlay)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DispatchSite] Highlight enabled but no overlay material is set on %s (Offer=%d MissionActive=%d Pulse=%d)"),
                *GetName(), bOfferActive ? 1 : 0, bMissionActive ? 1 : 0, bOfferExpiredPulseActive ? 1 : 0);
        }

        for (UMeshComponent* M : cachedMeshes)
        {
            if (!M) continue;

            if (bEnabled)
            {
                if (bRestorePreviousOverlayMaterial && !previousOverlayByMesh.Contains(M))
                {
                    previousOverlayByMesh.Add(M, M->GetOverlayMaterial());
                }

                if (DesiredOverlay)
                {
                    M->SetOverlayMaterial(DesiredOverlay);
                }
            }
            else
            {
                if (bRestorePreviousOverlayMaterial)
                {
                    if (TObjectPtr<UMaterialInterface>* Prev = previousOverlayByMesh.Find(M))
                    {
                        M->SetOverlayMaterial(Prev->Get());
                        previousOverlayByMesh.Remove(M);
                    }
                    else
                    {
                        M->SetOverlayMaterial(nullptr);
                    }
                }
                else
                {
                    M->SetOverlayMaterial(nullptr);
                }
            }
        }
    }

    // 2) CustomDepth outline highlight (optional secondary).
    if (bUseCustomDepthHighlight)
    {
        for (UPrimitiveComponent* P : cachedPrimitives)
        {
            if (!P) continue;

            P->SetRenderCustomDepth(bEnabled);

            // Only set stencil when enabled to avoid overwriting other systems when disabled.
            if (bEnabled)
            {
                P->SetCustomDepthStencilValue(Stencil);
            }
        }
    }
}


#pragma endregion INTERNAL
