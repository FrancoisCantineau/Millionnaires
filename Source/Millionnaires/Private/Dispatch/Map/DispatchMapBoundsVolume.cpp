/* 
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchMapBoundsVolume" - Source
 * Notes: Defines world bounds used to convert world locations to normalized map positions.
 */
#include "Dispatch/Map/DispatchMapBoundsVolume.h"

#include "Components/BoxComponent.h"

ADispatchMapBoundsVolume::ADispatchMapBoundsVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    boundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBox"));
    SetRootComponent(boundsBox);
    boundsBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    boundsBox->SetHiddenInGame(true);
}

FVector ADispatchMapBoundsVolume::GetWorldMin() const
{
    const FVector Center = boundsBox ? boundsBox->GetComponentLocation() : GetActorLocation();
    const FVector Extent = boundsBox ? boundsBox->GetScaledBoxExtent() : FVector(1000.f);
    return Center - Extent;
}

FVector ADispatchMapBoundsVolume::GetWorldMax() const
{
    const FVector Center = boundsBox ? boundsBox->GetComponentLocation() : GetActorLocation();
    const FVector Extent = boundsBox ? boundsBox->GetScaledBoxExtent() : FVector(1000.f);
    return Center + Extent;
}

FVector ADispatchMapBoundsVolume::GetWorldCenter() const
{
    return boundsBox ? boundsBox->GetComponentLocation() : GetActorLocation();
}
