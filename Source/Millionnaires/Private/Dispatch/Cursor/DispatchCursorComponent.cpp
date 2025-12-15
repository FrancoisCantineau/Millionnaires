/* 
 * Millionnaires Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCursorComponent" - Source
 * Notes: Cursor traces, hover radial charge and slow motion implementation.
 */
#include "Dispatch/Cursor/DispatchCursorComponent.h"

#include "Dispatch/DispatchPlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#pragma region LIFECYCLE

UDispatchCursorComponent::UDispatchCursorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDispatchCursorComponent::BeginPlay()
{
    Super::BeginPlay();

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

    AActor* PreviousHovered = HoveredActor.Get();
    AActor* NewHovered = PerformHoverTrace();

    if (PreviousHovered != NewHovered)
    {
        CurrentHoverProgress = 0.f;
        HoveredActor = NewHovered;
    }

    UpdateHoverAndSlowMo(DeltaTime, NewHovered);
}

#pragma endregion LIFECYCLE

#pragma region API

void UDispatchCursorComponent::HandleClick()
{
    AActor* Target = HoveredActor.Get();
    OnActorClicked.Broadcast(Target);

    // Here you can optionally call an interaction interface on the hovered actor.
    // Example (pseudo-code, depends on your existing interface).
}

void UDispatchCursorComponent::SetWorldCursorEnabled(bool bEnabled)
{
    bWorldCursorEnabled = bEnabled;

    if (!bWorldCursorEnabled)
    {
        // Reset state and restore time immediately.
        HoveredActor.Reset();
        CurrentHoverProgress = 0.f;
        CurrentTimeDilation = 1.f;
        ApplyGlobalTimeDilation(1.f);

        // Force UI to hide radial instantly (it will fade out via UIManager).
        OnHoverProgress.Broadcast(0.f, false);
        OnHoverUIProgress.Broadcast(0.f, LastMousePos, false);
    }
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

    float X = 0.f;
    float Y = 0.f;
    if (CachedController->GetMousePosition(X, Y))
    {
        LastMousePos = FVector2D(X, Y);
    }

    FVector WorldOrigin;
    FVector WorldDir;
    if (!CachedController->DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
    {
        HoveredActor.Reset();
        return nullptr;
    }

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = WorldOrigin + (WorldDir * TraceDistance);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DispatchCursorTrace), false);
    Params.bReturnPhysicalMaterial = false;
    Params.AddIgnoredActor(CachedController.Get());

    if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, Params))
    {
        return Hit.GetActor();
    }
    else
    {
        return nullptr;
    }

    return nullptr;
}

void UDispatchCursorComponent::UpdateHoverAndSlowMo(float DeltaTime, AActor* NewHoveredActor)
{
    const bool bHasHover = (NewHoveredActor != nullptr);
    const bool bIsCharacter = bHasHover && NewHoveredActor->IsA<APawn>();
    const bool bIsHoverable =
        bHasHover &&
        (bIsCharacter || NewHoveredActor->ActorHasTag(HoverableTag));

    if (HoverFillTime <= KINDA_SMALL_NUMBER)
    {
        HoverFillTime = 0.1f;
    }

    // Update radial progress (fill or drain).
    const float TargetProgress = bIsHoverable ? 1.f : 0.f;
    const float Speed = 1.f / HoverFillTime;
    CurrentHoverProgress = FMath::FInterpConstantTo(CurrentHoverProgress, TargetProgress, DeltaTime, Speed);
    CurrentHoverProgress = FMath::Clamp(CurrentHoverProgress, 0.f, 1.f);

    // Time slowdown only when hovering a character.
    float TargetDilation = 1.f;
    if (bIsCharacter && CurrentHoverProgress > 0.f)
    {
        TargetDilation = FMath::Lerp(1.f, SlowMoTargetDilation, CurrentHoverProgress);
    }

    CurrentTimeDilation = FMath::FInterpTo(CurrentTimeDilation, TargetDilation, DeltaTime, SlowMoInterpSpeed);
    ApplyGlobalTimeDilation(CurrentTimeDilation);

    // Legacy broadcast
    OnHoverProgress.Broadcast(CurrentHoverProgress, bIsCharacter);

    // UI broadcast
    OnHoverUIProgress.Broadcast(CurrentHoverProgress, LastMousePos, bIsHoverable);
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
