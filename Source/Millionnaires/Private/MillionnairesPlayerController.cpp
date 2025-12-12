/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Source
 * Notes: Player controller managing input mapping, dispatch/mission phases and mouse/camera setup.
 */

#include "MillionnairesPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MillionnairesCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Millionnaires.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "Systems/CharacterSelectionSubsystem.h"
#include "Data/CharacterDefinition.h"
#include "Characters/Player/PlayerCharacter.h"

AMillionnairesPlayerController::AMillionnairesPlayerController()
{
    // Set the player camera manager class
    PlayerCameraManagerClass = AMillionnairesCameraManager::StaticClass();
}

// -------------------------------------------------
//  CONTROLLER OVERRIDES
// -------------------------------------------------
#pragma region CONTROLLER_OVERRIDES

void AMillionnairesPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Start in dispatch phase (top-down camera, mouse available).
    EnterDispatchPhase();

    // Bind to character selection subsystem so we can react when a character is chosen.
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UCharacterSelectionSubsystem* SelectionSubsystem = GameInstance->GetSubsystem<UCharacterSelectionSubsystem>())
        {
            SelectionSubsystem->OnSelectedCharacterChanged.AddDynamic(this, &AMillionnairesPlayerController::HandleSelectedCharacterChanged);
        }
    }
}

void AMillionnairesPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Only add IMCs for local player controllers
    if (IsLocalPlayerController())
    {
        // Add Input Mapping Context
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            {
                if (CurrentContext)
                {
                    Subsystem->AddMappingContext(CurrentContext, 0);
                }
            }

            // Only add these IMCs if we're not using mobile touch input
            if (!ShouldUseTouchControls())
            {
                for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
                {
                    if (CurrentContext)
                    {
                        Subsystem->AddMappingContext(CurrentContext, 0);
                    }
                }
            }
        }
    }
}

#pragma endregion

// -------------------------------------------------
//  TOUCH INTERNAL
// -------------------------------------------------
#pragma region TOUCH_INTERNAL

bool AMillionnairesPlayerController::ShouldUseTouchControls() const
{
    // are we on a mobile platform? Should we force touch?
    return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

#pragma endregion

// -------------------------------------------------
//  PHASE API
// -------------------------------------------------
#pragma region PHASE_API

void AMillionnairesPlayerController::InitializeDispatchCamera()
{
    if (DispatchCamera.IsValid())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsWithTag(World, DispatchCameraTag, FoundCameras);

    if (FoundCameras.Num() > 0)
    {
        DispatchCamera = FoundCameras[0];
        UE_LOG(LogMillionnaires, Log, TEXT("[PC] Dispatch camera found: %s"), *DispatchCamera->GetName());
    }
    else
    {
        UE_LOG(LogMillionnaires, Warning, TEXT("[PC] No dispatch camera found with tag '%s'."), *DispatchCameraTag.ToString());
    }
}

void AMillionnairesPlayerController::EnterDispatchPhase()
{
    CurrentPhase = EMillionairesGamePhase::Dispatch;

    InitializeDispatchCamera();

    // Use dispatch camera if available, otherwise fall back to pawn
    if (DispatchCamera.IsValid())
    {
        SetViewTarget(DispatchCamera.Get());
    }
    else if (APawn* ControlledPawn = GetPawn())
    {
        SetViewTarget(ControlledPawn);
    }

    // Mouse + click enabled for selection
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    UE_LOG(LogMillionnaires, Log, TEXT("[PC] Entered Dispatch phase."));
}

void AMillionnairesPlayerController::EnterMissionPhase(UCharacterDefinition* SelectedCharacter)
{
    CurrentPhase = EMillionairesGamePhase::Mission;
    LastSelectedCharacter = SelectedCharacter;

    // Apply definition to the current player pawn if possible
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
    {
        if (SelectedCharacter)
        {
            PlayerCharacter->ApplyCharacterDefinition(SelectedCharacter);
        }

        // Blend camera from dispatch view target to the player pawn
        SetViewTargetWithBlend(PlayerCharacter, PhaseTransitionBlendTime);
    }
    else if (APawn* ControlledPawn = GetPawn())
    {
        SetViewTargetWithBlend(ControlledPawn, PhaseTransitionBlendTime);
    }

    // Switch input mode to pure gameplay (no cursor)
    bShowMouseCursor = false;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

    UE_LOG(LogMillionnaires, Log, TEXT("[PC] Entered Mission phase with character '%s'."),
        SelectedCharacter ? *SelectedCharacter->GetDisplayName().ToString() : TEXT("None"));
}

void AMillionnairesPlayerController::HandleSelectedCharacterChanged(UCharacterDefinition* NewSelection)
{
    // We only auto-start the mission the first time a character is chosen during Dispatch.
    if (CurrentPhase != EMillionairesGamePhase::Dispatch)
    {
        return;
    }

    if (!NewSelection)
    {
        return;
    }

    UE_LOG(LogMillionnaires, Log, TEXT("[PC] Character selected in dispatch: '%s'."),
        *NewSelection->GetDisplayName().ToString());

    // Immediately transition to mission phase with this character.
    EnterMissionPhase(NewSelection);
}

#pragma endregion
