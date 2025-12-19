/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "DispatchCameraSpot" - Source
 * Notes: Implementation of the Dispatch camera spot actor.
 */

#include "Dispatch/Camera/DispatchCameraSpot.h"
#include "Dispatch/Camera/DispatchCameraManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ADispatchCameraSpot::ADispatchCameraSpot()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Root);
}

void ADispatchCameraSpot::BeginPlay()
{
    Super::BeginPlay();

    if (Camera)
    {
        InitialFOV = Camera->FieldOfView;
        
        // Cache neutral pose for mouse parallax.
        InitialCameraRelativeRotation = Camera->GetRelativeRotation();
        InitialCameraRelativeLocation = Camera->GetRelativeLocation();
    }

    RegisterToDispatchCameraManager();
}

void ADispatchCameraSpot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterFromDispatchCameraManager();

    Super::EndPlay(EndPlayReason);
}

void ADispatchCameraSpot::RegisterToDispatchCameraManager()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        return;
    }

    UDispatchCameraManagerComponent* Manager = PC->FindComponentByClass<UDispatchCameraManagerComponent>();
    if (Manager)
    {
        Manager->RegisterCameraSpot(this);
    }
}

void ADispatchCameraSpot::UnregisterFromDispatchCameraManager()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        return;
    }

    UDispatchCameraManagerComponent* Manager = PC->FindComponentByClass<UDispatchCameraManagerComponent>();
    if (Manager)
    {
        Manager->UnregisterCameraSpot(this);
    }
}
