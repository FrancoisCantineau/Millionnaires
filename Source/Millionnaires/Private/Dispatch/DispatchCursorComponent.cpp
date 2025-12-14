/**
 * Millionnaires Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorComponent"
 * Notes: Cursor traces, hover radial charge and slow motion implementation.
 */

#include "Dispatch/DispatchCursorComponent.h"

#include "Dispatch/DispatchPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

UDispatchCursorComponent::UDispatchCursorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    TraceDistance = 100000.f;
    TraceChannel = ECC_Visibility;

    HoverableTag = FName(TEXT("DispatchHover"));
    MapHologramTag = FName(TEXT("DispatchMapHologram"));

    HoverFillTime = 1.0f;
    SlowMoTargetDilation = 0.1f;
    SlowMoInterpSpeed = 3.0f;

    CurrentHoverProgress = 0.f;
    CurrentTimeDilation = 1.f;
}

#pragma region LIFECYCLE

void UDispatchCursorComponent::BeginPlay()
{
    Super::BeginPlay();

    CachedController = Cast<ADispatchPlayerController>(GetOwner());

    // Ensure we start at normal speed.
    ApplyGlobalTimeDilation(1.f);
}

void UDispatchCursorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Restore normal time when leaving the Dispatch scene.
    ApplyGlobalTimeDilation(1.f);

    Super::EndPlay(EndPlayReason);
}

void UDispatchCursorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* NewHoveredActor = PerformHoverTrace();
    UpdateHoverAndSlowMo(DeltaTime, NewHoveredActor);
}

#pragma endregion LIFECYCLE

#pragma region API

void UDispatchCursorComponent::HandleClick()
{
    AActor* Target = HoveredActor.Get();
    if (!Target)
    {
        return;
    }

    // Here you could optionally call an interaction interface on the hovered actor.
    // Example (pseudo-code, depends on your existing interface):
    //
    // if (Target->GetClass()->ImplementsInterface(UDispatchInteractableInterface::StaticClass()))
    // {
    //     IDispatchInteractableInterface::Execute_OnDispatchClicked(Target, CachedController.Get());
    // }
}

#pragma endregion API

#pragma region INTERNAL

AActor* UDispatchCursorComponent::PerformHoverTrace()
{
    if (!CachedController.IsValid())
    {
        CachedController = Cast<ADispatchPlayerController>(GetOwner());
        if (!CachedController.IsValid())
        {
            HoveredActor.Reset();
            return nullptr;
        }
    }

    FVector2D MousePos(0.f, 0.f);
    if (!CachedController->GetMousePosition(MousePos.X, MousePos.Y))
    {
        HoveredActor.Reset();
        return nullptr;
    }

    FVector WorldOrigin;
    FVector WorldDirection;
    if (!CachedController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
    {
        HoveredActor.Reset();
        return nullptr;
    }

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = TraceStart + WorldDirection * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DispatchCursorTrace), false);
    Params.bReturnPhysicalMaterial = false;
    Params.AddIgnoredActor(CachedController.Get());

    if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, Params))
    {
        HoveredActor = Hit.GetActor();
    }
    else
    {
        HoveredActor.Reset();
    }

    return HoveredActor.Get();
}

void UDispatchCursorComponent::UpdateHoverAndSlowMo(float DeltaTime, AActor* NewHoveredActor)
{
    const bool bHasHover = (NewHoveredActor != nullptr);
    const bool bIsCharacter = bHasHover && NewHoveredActor->IsA<APawn>();
    const bool bIsHoverable =
        bHasHover &&
        (bIsCharacter || NewHoveredActor->ActorHasTag(HoverableTag) || NewHoveredActor->ActorHasTag(MapHologramTag));

    if (HoverFillTime <= KINDA_SMALL_NUMBER)
    {
        HoverFillTime = 0.1f;
    }

    // Update radial progress (fill or drain).
    if (bIsHoverable)
    {
        const float DeltaProgress = DeltaTime / HoverFillTime;
        CurrentHoverProgress = FMath::Clamp(CurrentHoverProgress + DeltaProgress, 0.f, 1.f);
    }
    else
    {
        const float DeltaProgress = DeltaTime / HoverFillTime;
        CurrentHoverProgress = FMath::Clamp(CurrentHoverProgress - DeltaProgress, 0.f, 1.f);
    }

    // Compute target time dilation.
    float TargetDilation = 1.f;

    // Time slowdown only when hovering a character.
    if (bIsCharacter && CurrentHoverProgress > 0.f)
    {
        TargetDilation = FMath::Lerp(1.f, SlowMoTargetDilation, CurrentHoverProgress);
    }

    // Smoothly interpolate towards the target dilation.
    CurrentTimeDilation = FMath::FInterpTo(CurrentTimeDilation, TargetDilation, DeltaTime, SlowMoInterpSpeed);
    ApplyGlobalTimeDilation(CurrentTimeDilation);

    // Notify listeners (e.g. the PlayerController updating the radial widget).
    OnHoverProgress.Broadcast(CurrentHoverProgress, bIsCharacter);
}

void UDispatchCursorComponent::ApplyGlobalTimeDilation(float NewDilation)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    NewDilation = FMath::Clamp(NewDilation, 0.01f, 5.f);
    UGameplayStatics::SetGlobalTimeDilation(World, NewDilation);
}

#pragma endregion INTERNAL
