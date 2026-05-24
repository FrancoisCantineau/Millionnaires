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
#include "Data/CharacterDefinition.h"
#include "EnhancedInputComponent.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Characters/Player/Data/Enum/PlayerEnum.h"
#include "UI/InputChallengeWidget.h"
#include "InputActionValue.h"
#include "Characters/Player/MillionnairePlayerBase.h"

AMillionnairesPlayerController::AMillionnairesPlayerController()
{
    PlayerCameraManagerClass = AMillionnairesCameraManager::StaticClass();
    ChallengeComponent = CreateDefaultSubobject<UInputChallengeComponent>(TEXT("ChallengeComponent"));
}

// -------------------------------------------------
//  HELPERS
// -------------------------------------------------

AMillionnairePlayerBase* AMillionnairesPlayerController::GetPlayerPawn() const
{
    return Cast<AMillionnairePlayerBase>(GetPawn());
}

// -------------------------------------------------
//  PLAYER MODE
// -------------------------------------------------

void AMillionnairesPlayerController::SetPlayerMode(EPlayerMode NewMode, AActor* ContextActor)
{
    CurrentMode = NewMode;
    BlockedActions.Empty();

    if (Cam)
    {
        Cam->SetRelativeRotation(OriginalCameraRotation);
        Cam = nullptr;
        OriginalCameraRotation = FRotator::ZeroRotator;
    }

    switch (CurrentMode)
    {
    case EPlayerMode::Gameplay:
        {
            SetIgnoreMoveInput(false);
            SetIgnoreLookInput(false);
            if (GetPawn())
                SetViewTargetWithBlend(GetPawn(), 0.3f);
        }
        break;

    case EPlayerMode::Inspect:
        {
            SetIgnoreMoveInput(true);
            InspectRotation = FRotator::ZeroRotator;

            BlockedActions.Add(EPlayerAction::Move);
            BlockedActions.Add(EPlayerAction::ToggleFlashlight);
            BlockedActions.Add(EPlayerAction::Interact);
            BlockedActions.Add(EPlayerAction::Jump);

            if (ContextActor)
            {
                SetViewTargetWithBlend(ContextActor, 0.3f);
                SetViewTarget(ContextActor);

                Cam = ContextActor->FindComponentByClass<UCameraComponent>();
                OriginalCameraRotation = Cam->GetRelativeRotation();
                InspectBaseRotation = OriginalCameraRotation;
                InspectRotation = FRotator::ZeroRotator;
                InspectBaseRotation = Cam ? Cam->GetComponentRotation() : ContextActor->GetActorRotation();
            }
        }
        break;

    case EPlayerMode::InputChallenge:
        {
            SetIgnoreMoveInput(true);
            SetIgnoreLookInput(true);

            BlockedActions.Add(EPlayerAction::Move);
            BlockedActions.Add(EPlayerAction::Jump);
            BlockedActions.Add(EPlayerAction::Interact);
            BlockedActions.Add(EPlayerAction::ToggleFlashlight);
        }
        break;
    }
}

void AMillionnairesPlayerController::HandleLookInput(FVector2D Value)
{
    switch (CurrentMode)
    {
    case EPlayerMode::Gameplay:
        AddYawInput(Value.X);
        AddPitchInput(Value.Y);
        break;

    case EPlayerMode::Inspect:
        {
            const float Sensitivity = .8f;

            InspectRotation.Yaw   += Value.X * Sensitivity;
            InspectRotation.Pitch += Value.Y * Sensitivity;

            InspectRotation.Yaw   = FMath::Clamp(InspectRotation.Yaw,   -15.f, 15.f);
            InspectRotation.Pitch = FMath::Clamp(InspectRotation.Pitch, -15.f, 15.f);

            if (!GetViewTarget()) return;

            if (Cam)
            {
                FRotator FinalRot = InspectBaseRotation + InspectRotation;
                Cam->SetWorldRotation(FinalRot);
            }
        }
        break;

    default:
        break;
    }
}

bool AMillionnairesPlayerController::CanPerform(EPlayerAction Action) const
{
    return !BlockedActions.Contains(Action);
}

// -------------------------------------------------
//  CHALLENGE
// -------------------------------------------------

void AMillionnairesPlayerController::StartInputChallenge(UInputChallengeDefinition* Definition)
{
    if (!Definition) return;
    ChallengeComponent->StartChallenge(Definition);
}

void AMillionnairesPlayerController::OnChallengeBegan(UInputChallengeDefinition* Definition)
{
    SetPlayerMode(EPlayerMode::InputChallenge, nullptr);

    if (!ChallengeWidgetClass) return;

    ChallengeWidget = CreateWidget<UInputChallengeWidget>(
        this,
        ChallengeWidgetClass
    );

    if (ChallengeWidget)
    {
        ChallengeWidget->AddToViewport();
        ChallengeWidget->Init(ChallengeComponent);
    }
}

void AMillionnairesPlayerController::OnChallengeOver()
{
    SetPlayerMode(EPlayerMode::Gameplay, nullptr);

    SetPlayerMode(EPlayerMode::Gameplay, nullptr);

    if (ChallengeWidget)
    {
        ChallengeWidget->RemoveFromParent();
        ChallengeWidget = nullptr;
    }
}

// -------------------------------------------------
//  CONTROLLER OVERRIDES
// -------------------------------------------------
#pragma region CONTROLLER_OVERRIDES

void AMillionnairesPlayerController::BeginPlay()
{
    Super::BeginPlay();

    ChallengeComponent->OnChallengeStarted.AddDynamic(this, &AMillionnairesPlayerController::OnChallengeBegan);
    ChallengeComponent->OnChallengeEnded.AddDynamic(this,   &AMillionnairesPlayerController::OnChallengeOver);

    if (DispatchCamera.IsValid())
        EnterDispatchPhase();
    else
        EnterMissionPhase(nullptr);
}

void AMillionnairesPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!IsLocalPlayerController()) return;

    // --- IMCs ---
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        for (UInputMappingContext* Ctx : DefaultMappingContexts)
            if (Ctx) Subsystem->AddMappingContext(Ctx, 0);

        if (!ShouldUseTouchControls())
            for (UInputMappingContext* Ctx : MobileExcludedMappingContexts)
                if (Ctx) Subsystem->AddMappingContext(Ctx, 0);
    }

    // --- Bindings ---
    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Existant
        if (ExitAction)
            EI->BindAction(ExitAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::HandleExitPressed);

        // Mouvement / caméra
        if (MoveAction)
            EI->BindAction(MoveAction, ETriggerEvent::Triggered, this,
                &AMillionnairesPlayerController::OnMoveInput);

        if (LookAction)
            EI->BindAction(LookAction, ETriggerEvent::Triggered, this,
                &AMillionnairesPlayerController::OnLookInput);

        if (MouseLookAction)
            EI->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
                &AMillionnairesPlayerController::OnLookInput);

        // Jump
        if (JumpAction)
        {
            EI->BindAction(JumpAction, ETriggerEvent::Started,   this,
                &AMillionnairesPlayerController::OnJumpStarted);
            EI->BindAction(JumpAction, ETriggerEvent::Completed, this,
                &AMillionnairesPlayerController::OnJumpCompleted);
        }

        // Gameplay
        if (InteractAction)
            EI->BindAction(InteractAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnInteractPressed);

        if (InventoryAction)
            EI->BindAction(InventoryAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnInventoryPressed);

        if (DropItemAction)
            EI->BindAction(DropItemAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnDropItemPressed);

        // Consommables
        if (UseHealthAction)
            EI->BindAction(UseHealthAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnUseHealthPressed);

        if (UseFoodAction)
            EI->BindAction(UseFoodAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnUseFoodPressed);

        if (UseBatteryAction)
            EI->BindAction(UseBatteryAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnUseBatteryPressed);

        // Flashlight
        if (ToggleFlashlightAction)
            EI->BindAction(ToggleFlashlightAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnToggleFlashlightPressed);

        if (EquipFlashlightAction)
            EI->BindAction(EquipFlashlightAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::OnEquipFlashlightPressed);

    }
}

#pragma endregion

// -------------------------------------------------
//  INPUT HANDLERS
// -------------------------------------------------
#pragma region INPUT_HANDLERS

void AMillionnairesPlayerController::OnMoveInput(const FInputActionValue& Value)
{
    if (RouteAction(MoveAction))
        return;
    
    if (!CanPerform(EPlayerAction::Move)) return;

    FVector2D V = Value.Get<FVector2D>();
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoMove(V.X, V.Y);
}

void AMillionnairesPlayerController::OnLookInput(const FInputActionValue& Value)
{
    FVector2D V = Value.Get<FVector2D>();
    HandleLookInput(V);
}

void AMillionnairesPlayerController::OnJumpStarted()
{
    if (RouteAction(JumpAction))
        return;

    if (!CanPerform(EPlayerAction::Jump)) return;
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoJumpStart();
}

void AMillionnairesPlayerController::OnJumpCompleted()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoJumpEnd();
}

void AMillionnairesPlayerController::OnInteractPressed()
{
    if (RouteAction(InteractAction))
        return;
    
    if (!CanPerform(EPlayerAction::Interact)) return;

    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoInteract();
}

void AMillionnairesPlayerController::OnInventoryPressed()
{
    if (RouteAction(InventoryAction))
        return;
    
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnInventoryPressed();
}

void AMillionnairesPlayerController::OnDropItemPressed()
{
    if (RouteAction(DropItemAction))
        return;
    
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnDropItemPressed();
}

void AMillionnairesPlayerController::OnUseHealthPressed()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnUseHealthPressed();
}

void AMillionnairesPlayerController::OnUseFoodPressed()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnUseFoodPressed();
}

void AMillionnairesPlayerController::OnUseBatteryPressed()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnUseBatteryPressed();
}

void AMillionnairesPlayerController::OnToggleFlashlightPressed()
{
    if (!CanPerform(EPlayerAction::ToggleFlashlight)) return;
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnToggleFlashlightPressed();
}

void AMillionnairesPlayerController::OnEquipFlashlightPressed()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->OnEquipFlashlightPressed();
}

void AMillionnairesPlayerController::HandleExitPressed()
{
    if (CurrentMode == EPlayerMode::Gameplay) return;
    SetPlayerMode(EPlayerMode::Gameplay, nullptr);
}

bool AMillionnairesPlayerController::RouteAction(UInputAction* Action)
{
    if (CurrentMode == EPlayerMode::InputChallenge && ChallengeComponent->IsActived())
    {
        ChallengeComponent->HandleInputAction(Action);
            return true; 
    }

    return false;
}

#pragma endregion

// -------------------------------------------------
//  TOUCH INTERNAL
// -------------------------------------------------
#pragma region TOUCH_INTERNAL

bool AMillionnairesPlayerController::ShouldUseTouchControls() const
{
    return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

#pragma endregion

// -------------------------------------------------
//  PHASE API
// -------------------------------------------------
#pragma region PHASE_API

void AMillionnairesPlayerController::InitializeDispatchCamera()
{
    if (DispatchCamera.IsValid()) return;

    UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsWithTag(World, DispatchCameraTag, FoundCameras);

    if (FoundCameras.Num() > 0)
    {
        DispatchCamera = FoundCameras[0];
        UE_LOG(LogMillionnaires, Log, TEXT("[PC] Dispatch camera found: %s"), *DispatchCamera->GetName());
    }
    else
    {
        UE_LOG(LogMillionnaires, Warning, TEXT("[PC] No dispatch camera found with tag '%s'."),
            *DispatchCameraTag.ToString());
    }
}

void AMillionnairesPlayerController::EnterDispatchPhase()
{
    CurrentPhase = EMillionairesGamePhase::Dispatch;
    InitializeDispatchCamera();

    if (DispatchCamera.IsValid())
        SetViewTarget(DispatchCamera.Get());
    else if (APawn* P = GetPawn())
        SetViewTarget(P);

    bShowMouseCursor     = true;
    bEnableClickEvents   = true;
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

    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
    {
        if (SelectedCharacter)
            PlayerCharacter->ApplyCharacterDefinition(SelectedCharacter);

        SetViewTargetWithBlend(PlayerCharacter, PhaseTransitionBlendTime);
    }
    else if (APawn* P = GetPawn())
    {
        SetViewTargetWithBlend(P, PhaseTransitionBlendTime);
    }

    bShowMouseCursor       = false;
    bEnableClickEvents     = false;
    bEnableMouseOverEvents = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

    UE_LOG(LogMillionnaires, Log, TEXT("[PC] Entered Mission phase with character '%s'."),
        SelectedCharacter ? *SelectedCharacter->GetDisplayName().ToString() : TEXT("None"));
}

void AMillionnairesPlayerController::HandleSelectedCharacterChanged(UCharacterDefinition* NewSelection)
{
    if (CurrentPhase != EMillionairesGamePhase::Dispatch) return;
    if (!NewSelection) return;

    UE_LOG(LogMillionnaires, Log, TEXT("[PC] Character selected in dispatch: '%s'."),
        *NewSelection->GetDisplayName().ToString());

    EnterMissionPhase(NewSelection);
}

#pragma endregion