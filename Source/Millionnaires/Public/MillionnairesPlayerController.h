/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "MillionnairesPlayerController" - Header
 * Notes: Player controller managing input mapping, dispatch/mission phases and mouse/camera setup.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MillionnairesPlayerController.generated.h"

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
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 *  Also handles transitions between Dispatch (top-down) and Mission (first person) phases.
 */
UCLASS(abstract, config = "Game")
class MILLIONNAIRES_API AMillionnairesPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    /** Constructor */
    AMillionnairesPlayerController();

protected:

    // -------------------------------------------------
    //  INPUT MAPPING
    // -------------------------------------------------
#pragma region INPUT_MAPPING

    /** Input Mapping Contexts always added for this controller. */
    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    /** Input Mapping Contexts that are excluded when using mobile touch input. */
    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

#pragma endregion

    // -------------------------------------------------
    //  TOUCH CONTROLS
    // -------------------------------------------------
#pragma region TOUCH_CONTROLS

    /** Mobile controls widget to spawn */
    UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    /** Pointer to the mobile controls widget */
    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

    /** If true, the player will use UMG touch controls even if not playing on mobile platforms */
    UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
    bool bForceTouchControls = false;

#pragma endregion

    // -------------------------------------------------
    //  GAME PHASE / CAMERA
    // -------------------------------------------------
#pragma region PHASE_LOGIC

    /** Current high-level game phase (Dispatch or Mission). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase|Runtime", meta = (ToolTip = "Current high-level game phase for this player."))
    EMillionairesGamePhase CurrentPhase = EMillionairesGamePhase::Dispatch;

    /** Tag used to find the dispatch (top-down) camera actor in the world. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase|Camera", meta = (ToolTip = "Tag used to locate the dispatch phase camera in the level."))
    FName DispatchCameraTag = FName("DispatchCamera");

    /** Blend time used when switching from dispatch camera to mission (first person) camera. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase|Camera", meta = (ClampMin = "0.0", ToolTip = "Blend time when switching between dispatch and mission camera views."))
    float PhaseTransitionBlendTime = 1.0f;

    /** Cached dispatch camera actor found by tag at BeginPlay. */
    UPROPERTY()
    TWeakObjectPtr<AActor> DispatchCamera;

    /** Last character definition selected while in dispatch phase. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase|Runtime", meta = (ToolTip = "Last character definition selected during dispatch."))
    TObjectPtr<UCharacterDefinition> LastSelectedCharacter = nullptr;

#pragma endregion

    // -------------------------------------------------
    //  APlayerController overrides
    // -------------------------------------------------
#pragma region CONTROLLER_OVERRIDES

    /** Gameplay initialization */
    virtual void BeginPlay() override;

    /** Input mapping context setup */
    virtual void SetupInputComponent() override;

#pragma endregion

    // -------------------------------------------------
    //  TOUCH INTERNAL
    // -------------------------------------------------
#pragma region TOUCH_INTERNAL

    /** Returns true if the player should use UMG touch controls */
    bool ShouldUseTouchControls() const;

#pragma endregion

    // -------------------------------------------------
    //  PHASE API
    // -------------------------------------------------
#pragma region PHASE_API

public:

    /** Enter dispatch phase (top-down camera, mouse visible, click to select characters). */
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void EnterDispatchPhase();

    /** Enter mission phase (first person control of the selected character). */
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void EnterMissionPhase(UCharacterDefinition* SelectedCharacter);

    /** Returns the current game phase. */
    UFUNCTION(BlueprintPure, Category = "Phase")
    EMillionairesGamePhase GetCurrentPhase() const { return CurrentPhase; }

protected:

    /** Finds and caches the dispatch camera using DispatchCameraTag. */
    void InitializeDispatchCamera();

    /** Called when the selected character changes in the CharacterSelectionSubsystem. */
    UFUNCTION()
    void HandleSelectedCharacterChanged(UCharacterDefinition* NewSelection);

#pragma endregion
};
