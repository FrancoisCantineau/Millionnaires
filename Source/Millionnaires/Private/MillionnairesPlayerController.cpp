/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Source
 * Notes: Player controller managing input mapping, dispatch/mission phases and mouse/camera setup.
 */

#include "MillionnairesPlayerController.h"

#include "ContextComponent.h"
#include "ContextInputRouterComponent.h"
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
#include "UI/InputChallengeWidget.h"
#include "InputActionValue.h"
#include "ContextCameraComponent.h"
#include "Characters/Player/MillionnairePlayerBase.h"

AMillionnairesPlayerController::AMillionnairesPlayerController()
{
    PlayerCameraManagerClass = AMillionnairesCameraManager::StaticClass();
    ChallengeComponent = CreateDefaultSubobject<UInputChallengeComponent>(TEXT("ChallengeComponent"));

    InputRouterComponent = CreateDefaultSubobject<UContextInputRouterComponent>(TEXT("InputRouterComponent"));
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


void AMillionnairesPlayerController::LockCamera(float YawRange, float PitchMin, float PitchMax)
{
    if (APlayerCameraManager* CM = PlayerCameraManager)
    {
        float CurrentYaw = GetControlRotation().Yaw;
        
        CM->ViewYawMin  = CurrentYaw - YawRange;
        CM->ViewYawMax  = CurrentYaw + YawRange;
        CM->ViewPitchMin = PitchMin;
        CM->ViewPitchMax = PitchMax;
    }
}

void AMillionnairesPlayerController::UnlockCamera()
{
    if (APlayerCameraManager* CM = PlayerCameraManager)
    {
        CM->ViewYawMin  = -359.f;
        CM->ViewYawMax  =  359.f;
        CM->ViewPitchMin = -89.f;
        CM->ViewPitchMax =  89.f;
    }
}


void AMillionnairesPlayerController::SetPlayerMode_Implementation(EPlayerMode NewMode, AActor* ContextActor)
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
            UnlockCamera();
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
    
    case EPlayerMode::Traversal:
        {
            LockCamera(75.f);
            
            BlockedActions.Add(EPlayerAction::Jump);
            BlockedActions.Add(EPlayerAction::Interact);
            BlockedActions.Add(EPlayerAction::ToggleFlashlight);
        }
    break;
}
}

void AMillionnairesPlayerController::HandleLookInput(FVector2D Value)
{
    if (CameraComp && CameraComp->MustReceiveLookInput())
    {
        CameraComp->ConsumeLookInput(Value);
        return;
    }
    UE_LOG(LogTemp, Warning,
    TEXT("ControlRot = %s"),
    *GetControlRotation().ToString());
    AddYawInput(Value.X);
    AddPitchInput(Value.Y);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "coucou");
     
}

bool AMillionnairesPlayerController::CanPerform_Implementation(EPlayerAction Action) const
{
    return !BlockedActions.Contains(Action);
}

// -------------------------------------------------
//  CHALLENGE
// -------------------------------------------------

void AMillionnairesPlayerController::StartInputChallenge(UInputChallengeDefinition* Definition)
{
    if (!ChallengeComponent || !Definition)
        return;

    ChallengeComponent->StartChallenge(Definition);
}

void AMillionnairesPlayerController::OnChallengeBegan(UInputChallengeDefinition* Definition)
{
    Execute_SetPlayerMode(this,EPlayerMode::InputChallenge, nullptr);

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
    Execute_SetPlayerMode(this,EPlayerMode::Gameplay, nullptr);
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

    if (ChallengeComponent)
    {
        ChallengeComponent->OnChallengeStarted.AddDynamic(
            this,
            &AMillionnairesPlayerController::OnChallengeBegan
        );

        ChallengeComponent->OnChallengeEnded.AddDynamic(
            this,
            &AMillionnairesPlayerController::OnChallengeOver
        );
    }

    if (DispatchCamera.IsValid())
        EnterDispatchPhase();
    else
        EnterMissionPhase(nullptr);

    if (APawn* Pawnn = GetPawn())
    {
        CameraComp = Pawnn->FindComponentByClass<UContextCameraComponent>();
    }
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
    
    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent))
    {
        TSet<TPair<const UInputAction*, ETriggerEvent>> BoundPairs;

        for (const UContextInputMappingDataAsset* DataAsset : InputDataAssets)
        {
            if (!DataAsset) continue;

            for (const FInputTagMapping& M : DataAsset->Mappings)
            {
                for (ETriggerEvent Event : M.Events)
                {
                    auto Key = MakeTuple(M.Action.Get(), Event);
                    if (!BoundPairs.Contains(Key))
                    {
                        BoundPairs.Add(Key);
                        EI->BindAction(M.Action, Event, this, &AMillionnairesPlayerController::OnInput);
                    }
                }
            }
        }
    
        /*
        // Existant
        if (ExitAction)
            EI->BindAction(ExitAction, ETriggerEvent::Started, this,
                &AMillionnairesPlayerController::HandleExitPressed);

        // Mouvement / caméra
        if (MoveAction)
            EI->BindAction(MoveAction, ETriggerEvent::Triggered, this,
                &AMillionnairesPlayerController::OnMoveInput);
            EI->BindAction(MoveAction, ETriggerEvent::Completed, this,
                &AMillionnairesPlayerController::OnMoveCompleted);
        
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
*/
    } 
}

void AMillionnairesPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UContextComponent* ContextComp =
        InPawn->FindComponentByClass<UContextComponent>();

    InputRouterComponent->Initialize(ContextComp);
}

void AMillionnairesPlayerController::OnInput(const FInputActionInstance& Instance)
{
    const UInputAction* Action = Instance.GetSourceAction();
    
    if (Action == LookAction || Action == MouseLookAction)
    {
        FVector2D V = Instance.GetValue().Get<FVector2D>();
        HandleLookInput(V);
        return;
    }

    if (InputRouterComponent)
        InputRouterComponent->HandleInputReceived(Instance);
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

    if (!Execute_CanPerform(this,EPlayerAction::Move))
        return;

    FVector2D V = Value.Get<FVector2D>();

    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoMove(V.X, V.Y);
}

void AMillionnairesPlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
    if (RouteAction(MoveAction))
        return;

    if (!Execute_CanPerform(this,EPlayerAction::Move))
        return;

    FVector2D V = Value.Get<FVector2D>();

    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoMoveEnd(V.X, V.Y);
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

    if (!Execute_CanPerform(this,EPlayerAction::Jump))
        return;

    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoJumpStart();
}

void AMillionnairesPlayerController::OnJumpCompleted()
{
    if (AMillionnairePlayerBase* P = GetPlayerPawn())
        P->DoJumpEnd();
}

// -------------------------------------------------
// ROUTING SYSTEM (CHALLENGE)
// -------------------------------------------------

bool AMillionnairesPlayerController::RouteAction(UInputAction* Action)
{
    if (CurrentMode == EPlayerMode::InputChallenge &&
        ChallengeComponent &&
        ChallengeComponent->IsActived())
    {
        ChallengeComponent->HandleInputPressed(Action);
        return true;
    }

    return false;
}

// -------------------------------------------------
// OTHER ACTIONS
// -------------------------------------------------

void AMillionnairesPlayerController::OnInteractPressed()
{
    if (RouteAction(InteractAction))
        return;

    if (!Execute_CanPerform(this,EPlayerAction::Interact))
        return;

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
    if (Execute_CanPerform(this,EPlayerAction::ToggleFlashlight))
        return;

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
    if (CurrentMode == EPlayerMode::Gameplay)
        return;
    
    Execute_SetPlayerMode(this,EPlayerMode::Gameplay, nullptr);
}

void AMillionnairesPlayerController::OnActionStarted(UInputAction* Action)
{
    if (RouteAction(Action))
        return;
}

void AMillionnairesPlayerController::OnActionCompleted(UInputAction* Action)
{
    if (RouteAction(Action))
        return;

    AMillionnairePlayerBase* P =
        Cast<AMillionnairePlayerBase>(GetPawn());

    if (!P) return;

    if (Action == JumpAction)
    {
        P->DoJumpEnd();
    }
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