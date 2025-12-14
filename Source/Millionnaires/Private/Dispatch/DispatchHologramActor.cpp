/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchHologramActor" - Source
 * Notes: Implementation of the example Dispatch hologram actor.
 */

#include "Dispatch/DispatchHologramActor.h"
#include "Dispatch/DispatchPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"

ADispatchHologramActor::ADispatchHologramActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;

    // Default collision so the cursor trace can hit this actor.
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    MeshComponent->SetGenerateOverlapEvents(false);
}

void ADispatchHologramActor::OnDispatchHoverStarted_Implementation(APlayerController* /*Controller*/)
{
    if (!MeshComponent)
    {
        return;
    }

    // Cache the default material on first hover if it was not assigned.
    if (!DefaultMaterial)
    {
        DefaultMaterial = MeshComponent->GetMaterial(0);
    }

    if (HoverMaterial)
    {
        MeshComponent->SetMaterial(0, HoverMaterial);
    }
}

void ADispatchHologramActor::OnDispatchHoverEnded_Implementation(APlayerController* /*Controller*/)
{
    if (!MeshComponent)
    {
        return;
    }

    if (DefaultMaterial)
    {
        MeshComponent->SetMaterial(0, DefaultMaterial);
    }
}

void ADispatchHologramActor::OnDispatchClicked_Implementation(APlayerController* Controller)
{
    OnHologramClicked.Broadcast();

    if (bAutoToggleMapOnClick && Controller)
    {
        if (ADispatchPlayerController* DispatchPC = Cast<ADispatchPlayerController>(Controller))
        {
            DispatchPC->ToggleMap();
        }
    }
}
