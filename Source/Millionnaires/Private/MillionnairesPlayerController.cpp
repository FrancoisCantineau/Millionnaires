/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Source
 * Notes: Player controller managing input routing, dispatch/mission phases and mouse/camera setup.
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

AMillionnairesPlayerController::AMillionnairesPlayerController()
{
    PlayerCameraManagerClass = AMillionnairesCameraManager::StaticClass();
    ChallengeComponent = CreateDefaultSubobject<UInputChallengeComponent>(TEXT("ChallengeComponent"));

    InputRouterComponent = CreateDefaultSubobject<UContextInputRouterComponent>(TEXT("InputRouterComponent"));
}

void AMillionnairesPlayerController::HandleLookInput(FVector2D Value)
{
    if (CameraComp && CameraComp->MustReceiveLookInput())
    {
        CameraComp->ConsumeLookInput(Value);
        return;
    }
    AddYawInput(Value.X);
    AddPitchInput(Value.Y);
}

// -------------------------------------------------
//  CONTROLLER OVERRIDES
// -------------------------------------------------
#pragma region CONTROLLER_OVERRIDES

void AMillionnairesPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
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
