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
    previousOverlayByMesh.Reset();

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

    // Re-apply current highlight state.
    const bool bShouldHighlight = bOfferActive || (currentMissionState != EDispatchMissionState::None
        && currentMissionState != EDispatchMissionState::Completed
        && currentMissionState != EDispatchMissionState::Failed
        && currentMissionState != EDispatchMissionState::Aborted);

    ApplyHighlight(bShouldHighlight, customDepthStencilValue);
}

void ADispatchMissionSiteActor::SetOfferActive(bool bActive)
{
    bOfferActive = bActive;

    if (cachedPrimitives.Num() == 0)
    {
        RebuildMeshCache();
    }

    // Offer highlight enabled unless mission already ended.
    const bool bEnable = bOfferActive;
    ApplyHighlight(bEnable, customDepthStencilValue);
}

void ADispatchMissionSiteActor::SetMissionState(EDispatchMissionState NewState)
{
    currentMissionState = NewState;

    if (cachedPrimitives.Num() == 0)
    {
        RebuildMeshCache();
    }

    const bool bEnded = (NewState == EDispatchMissionState::Completed || NewState == EDispatchMissionState::Failed || NewState == EDispatchMissionState::Aborted);
    ApplyHighlight(!bEnded, customDepthStencilValue);
}

#pragma endregion API

#pragma region INTERNAL

void ADispatchMissionSiteActor::ApplyHighlight(bool bEnabled, int32 Stencil)
{
    // 1) OverlayMaterial highlight (preferred).
    if (bUseOverlayMaterialHighlight)
    {
        if (!overlayHighlightMaterial)
        {
            // We still allow CustomDepth highlight even if overlay material is not set.
            UE_LOG(LogTemp, Warning, TEXT("[DispatchSite] Overlay highlight enabled but overlayHighlightMaterial is null on %s"), *GetName());
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

                if (overlayHighlightMaterial)
                {
                    M->SetOverlayMaterial(overlayHighlightMaterial);
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
                        // If we didn't cache anything (rare), clear it.
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
