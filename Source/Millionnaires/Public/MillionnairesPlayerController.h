/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Header
 * Notes: Player controller managing input routing, dispatch/mission phases and mouse/camera setup.
 *
 * CLEANUP NOTE: gameplay action handlers (Move/Jump/Interact/Inventory/DropItem/consumables/
 * Flashlight) used to live here, calling directly into the Pawn. They've since moved onto the
 * Pawn itself (it now receives its own input via ContextFramework directly) - removed from here
 * to avoid two places doing the same job. The Controller still OWNS the blocking authority
 * (SetPlayerMode/CanPerform via IControllerInterface) - the Pawn is expected to query
 * CanPerform() before acting, so that part stays here even though the action handlers moved.
 */

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"

//Input challenge

#include "InputChallengeComponent.h"
#include "InputChallengeDefinition.h"
#include "Controller/ControllerInterface.h"

#include "MillionnairesPlayerController.generated.h"

class UContextCameraComponent;
class UContextInputMappingDataAsset;
class UContextInputRouterComponent;
class UInputChallengeWidget;
enum class EPlayerAction : uint8;
class UCameraComponent;
class UInputMappingContext;
class UUserWidget;
class UCharacterDefinition;

/**
 * High-level game phases for Millionaire.
 */
UENUM(BlueprintType)
enum class EMillionairesGamePhase : uint8
{
    Dispatch    UMETA(DisplayName = "Dispatch"),
    Mission     UMETA(DisplayName = "Mission")
};

/**
 *  First person Player Controller
 *  Manages input routing (via ContextFramework) and the mode/blocking authority the Pawn queries.
 *  Overrides the Player Camera Manager class.
 *  Also handles transitions between Dispatch (top-down) and Mission (first person) phases.
 */
UCLASS(abstract, config = "Game")
class MILLIONNAIRES_API AMillionnairesPlayerController : public APlayerController, public IControllerInterface
{
    GENERATED_BODY()

public:

    /** Constructor */
    AMillionnairesPlayerController();

    UFUNCTION()
    void HandleLookInput(FVector2D Value);
    
    FORCEINLINE UInputChallengeComponent* GetChallengeComponent() const { return ChallengeComponent; }

protected:

    virtual void OnPossess(APawn* InPawn) override;

    // -------------------------------------------------
    //  INPUT ROUTING
    // -------------------------------------------------
#pragma region INPUT_ROUTING

    void OnInput(const FInputActionInstance& Instance);

    UPROPERTY(EditDefaultsOnly)
    TArray<UContextInputMappingDataAsset*> InputDataAssets;

    UPROPERTY()
    UContextCameraComponent* CameraComp = nullptr;

    UPROPERTY(EditAnywhere)
    UContextInputRouterComponent* InputRouterComponent;

    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    /** TODO: not currently bound to anything (was part of the removed legacy binding block) -
     *  decide whether "exit current mode" should be rebound through the InputDataAssets path,
     *  or dropped along with HandleExitPressed if it's no longer needed. */
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> ExitAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> MouseLookAction;

#pragma endregion

    // -------------------------------------------------
    //  MODE / EXIT
    // -------------------------------------------------
#pragma region MODE

    /** See ExitAction TODO above - currently unreachable, kept for when that's decided. */
    void HandleExitPressed();

#pragma endregion

    // -------------------------------------------------
    //  INSPECT / MODE
    // -------------------------------------------------
#pragma region INSPECT

    FRotator InspectRotation;
    FRotator InspectBaseRotation;
    FRotator OriginalCameraRotation;

    UPROPERTY()
    UCameraComponent* Cam;

    TArray<EPlayerAction> BlockedActions;

    void LockCamera(float YawRange = 75.f, float PitchMin = -89.f, float PitchMax = 89.f);
    void UnlockCamera();
#pragma endregion

    // -------------------------------------------------
    //  CHALLENGE
    // -------------------------------------------------
#pragma region CHALLENGE

    UPROPERTY(VisibleAnywhere, Category = "Challenge")
    TObjectPtr<UInputChallengeComponent> ChallengeComponent;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UInputChallengeWidget> ChallengeWidgetClass;

    UPROPERTY()
    UInputChallengeWidget* ChallengeWidget;

#pragma endregion

    // -------------------------------------------------
    //  TOUCH CONTROLS
    // -------------------------------------------------
#pragma region TOUCH_CONTROLS

    UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

    UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
    bool bForceTouchControls = false;

#pragma endregion

    // -------------------------------------------------
    //  PHASE
    // -------------------------------------------------
#pragma region PHASE_LOGIC

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase|Runtime")
    EMillionairesGamePhase CurrentPhase = EMillionairesGamePhase::Dispatch;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase|Camera")
    FName DispatchCameraTag = FName("DispatchCamera");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase|Camera", meta = (ClampMin = "0.0"))
    float PhaseTransitionBlendTime = 1.0f;

    UPROPERTY()
    TWeakObjectPtr<AActor> DispatchCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase|Runtime")
    TObjectPtr<UCharacterDefinition> LastSelectedCharacter = nullptr;

#pragma endregion

    // -------------------------------------------------
    //  CONTROLLER OVERRIDES
    // -------------------------------------------------
#pragma region CONTROLLER_OVERRIDES

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

#pragma endregion

    // -------------------------------------------------
    //  TOUCH INTERNAL
    // -------------------------------------------------
#pragma region TOUCH_INTERNAL

    bool ShouldUseTouchControls() const;

#pragma endregion

    // -------------------------------------------------
    //  PHASE API
    // -------------------------------------------------
#pragma region PHASE_API

public:

    UFUNCTION(BlueprintCallable, Category = "Phase")
    void EnterDispatchPhase();

    UFUNCTION(BlueprintCallable, Category = "Phase")
    void EnterMissionPhase(UCharacterDefinition* SelectedCharacter);

    UFUNCTION(BlueprintPure, Category = "Phase")
    EMillionairesGamePhase GetCurrentPhase() const { return CurrentPhase; }

protected:

    void InitializeDispatchCamera();

    UFUNCTION()
    void HandleSelectedCharacterChanged(UCharacterDefinition* NewSelection);

#pragma endregion

#pragma region PLAYERMODE

    EPlayerMode CurrentMode;

    UPROPERTY()
    AActor* CurrentInteractable;

#pragma endregion

};
