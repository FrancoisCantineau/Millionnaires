/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Header
 * Notes: Player controller managing input mapping, dispatch/mission phases and mouse/camera setup.
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
class AMillionnairePlayerBase;

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
 *  Manages the input mapping context.
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

    virtual void SetPlayerMode_Implementation(EPlayerMode NewMode, AActor* ContextActor = nullptr) override;
    virtual bool CanPerform_Implementation(EPlayerAction Action) const override;

    UFUNCTION(BlueprintCallable)
    void StartInputChallenge(UInputChallengeDefinition* Definition);

    FORCEINLINE UInputChallengeComponent* GetChallengeComponent() const { return ChallengeComponent; }

protected:

    virtual void OnPossess(APawn* InPawn) override;
    
    // -------------------------------------------------
    //  INPUT ACTIONS
    // -------------------------------------------------
#pragma region INPUT_ACTIONS

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
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> ExitAction;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> MouseLookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> InventoryAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> DropItemAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> UseHealthAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> UseFoodAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> UseBatteryAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> ToggleFlashlightAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> EquipFlashlightAction;

#pragma endregion

    // -------------------------------------------------
    //  INPUT HANDLERS
    // -------------------------------------------------
#pragma region INPUT_HANDLERS

    void OnMoveInput(const FInputActionValue& Value);
    void OnMoveCompleted(const FInputActionValue& Value);
    void OnLookInput(const FInputActionValue& Value);
    void OnJumpStarted();
    void OnJumpCompleted();
    void OnInteractPressed();
    void OnInventoryPressed();
    void OnDropItemPressed();
    void OnUseHealthPressed();
    void OnUseFoodPressed();
    void OnUseBatteryPressed();
    void OnToggleFlashlightPressed();
    void OnEquipFlashlightPressed();
    void HandleExitPressed();
    bool RouteAction(UInputAction* Action);
    void OnActionStarted(UInputAction* Action);
    void OnActionCompleted(UInputAction* Action);

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

    UFUNCTION()
    void OnChallengeBegan(UInputChallengeDefinition* Definition);

    UFUNCTION()
    void OnChallengeOver();

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

private:
    
    AMillionnairePlayerBase* GetPlayerPawn() const;
    
};

