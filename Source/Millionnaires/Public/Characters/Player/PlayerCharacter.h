/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
 * Last Updated by: "0nnen"
 * Class: "PlayerCharacter" - Header
 * Notes: Playable first-person character derived from BaseCharacter. Handles camera, input and applying character definitions.
 */

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UInputAction;
struct FInputActionValue;
class UCharacterDefinition;

UCLASS()
class MILLIONNAIRES_API APlayerCharacter : public ABaseCharacter
{
    GENERATED_BODY()

public:

    APlayerCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "Character|Components")
    USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

    UFUNCTION(BlueprintPure, Category = "Character|Components")
    UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    UFUNCTION(BlueprintCallable, Category = "Character|Definition")
    void ApplyCharacterDefinition(UCharacterDefinition* Definition);

protected:

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma region INPUT_HANDLERS
    /** Handles 2D movement input from enhanced input. */
    void HandleMove(const FInputActionValue& Value);

    /** Handles look/aim input from enhanced input (mouse or stick). */
    void HandleLook(const FInputActionValue& Value);

    /** Called when the jump input is pressed. */
    void HandleJumpStart();

    /** Called when the jump input is released. */
    void HandleJumpEnd();

    /** Called when the sprint input is pressed. */
    void HandleRunStart();

    /** Called when the sprint input is released. */
    void HandleRunEnd();

    /** Toggles crouch state on or off. */
    void HandleCrouchToggle();
#pragma endregion

#pragma region MOVEMENT_INTERNAL
    /** Computes the desired max walk speed from the current movement state (walk / run / crouch). */
    void UpdateMovementSpeed();

    /** Smoothly interpolates the CharacterMovement MaxWalkSpeed towards DesiredMaxWalkSpeed. */
    void SmoothUpdateMovementSpeed(float DeltaSeconds);

    /** Applies yaw/pitch input to the controller. */
    void ApplyAim(float YawInput, float PitchInput);

    /** Applies movement input along right and forward vectors. */
    void ApplyMovement(float RightInput, float ForwardInput);

    /** Applies meshes and anim blueprints from the provided character definition. */
    void ApplyVisualsFromDefinition(UCharacterDefinition* Definition);

#pragma endregion

#pragma region CAMERA_FEEL_INTERNAL

    /** Master update called each tick - orchestrates all camera feel sub-systems. */
    void UpdateCameraFeel(float DeltaSeconds);

    /** Procedural head bob driven by locomotion speed and state. */
    void UpdateHeadBob(float DeltaSeconds);

    /** Subtle idle breathing sway applied when the player is nearly still. */
    void UpdateBreathingSway(float DeltaSeconds);

    /** Dynamic FOV, slight push-out when sprinting, restore on walk/crouch. */
    void UpdateDynamicFOV(float DeltaSeconds);

    /** Checks if a footstep occurred this frame and triggers a camera shake if so. */
    void TryTriggerFootstepShake(float DeltaSeconds);

#pragma endregion

protected:

#pragma region MOVEMENTS_VARIABLES
    /** Walking speed used when the player is moving normally. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Walking speed used when the player is moving normally."))
    float WalkSpeed = 280.0f;

    /** Running speed used while the sprint input is held. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Running speed used while the sprint input is held."))
    float RunSpeed = 520.0f;

    /** Movement speed used while crouched. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Movement speed used while crouched."))
    float CrouchSpeed = 160.0f;

    /** Time in seconds needed to reach the desired max speed from the current speed. 0 = instant change. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Acceleration", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "3.0", ToolTip = "Time in seconds needed to reach the desired max speed from the current speed. 0 = instant change."))
    float TimeToReachMaxSpeed = 0.4f;
    
    /** Enable or disable debug logs for movement state changes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "If true, the player will log movement state changes (walk / run / crouch)."))
    bool bDebugMovement = false;

    /** True while sprint input is held. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "True while sprint input is held."))
    bool bIsSprinting = false;

    /** Desired target max walk speed computed from movement state. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "Desired target max walk speed computed from movement state."))
    float DesiredMaxWalkSpeed = 280.0f;

    /** Current interpolated max walk speed actually applied to CharacterMovementComponent. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "Current interpolated max walk speed actually applied to CharacterMovementComponent."))
    float CurrentMaxWalkSpeed = 280.0f;

#pragma endregion

#pragma region HEAD_BOB_VARIABLES

    /** Base frequency for head bob while walking in cycles/second. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.0", UIMax = "10.0"))
    float HeadBobFrequency = 3.0f;

    /** Vertical amplitude for head bob while walking in cm.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.0", UIMax = "20.0"))
    float HeadBobAmplitudeV = 6.0f;

    /** Lateral amplitude for head bob while walking in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.0", UIMax = "10.0"))
    float HeadBobAmplitudeH = 4.0f;

    /** Frequency multiplier for head bob while sprinting.  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "1.0", UIMax = "3.0"))
    float HeadBobSprintFrequencyMultiplier = 1.5f;

    /** Vertical amplitude multiplier for head bob while sprinting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "1.0", UIMax = "10.0"))
    float HeadBobSprintVerticalMultiplier = 7.0f;

    /** Lateral amplitude multiplier for head bob while sprinting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "1.0", UIMax = "8.0"))
    float HeadBobSprintLateralMultiplier = 8.0f;

    /** Roll amplitude for head bob in degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.0", UIMax = "15.0"))
    float HeadBobRollAmplitude = 8.0f;

    /** Pitch amplitude for head bob in degrees.*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.0", UIMax = "5.0"))
    float HeadBobPitchAmplitude = 2.0f;

    /** Interpolation speed for head bob rotation changes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.1", UIMax = "30.0"))
    float HeadBobRotationInterpSpeed = 6.0f;

    /** Interpolation speed for head bob position changes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|HeadBob",meta = (ClampMin = "0.1", UIMax = "20.0"))
    float HeadBobInterpSpeed = 4.0f;

#pragma endregion

#pragma region BREATHING_VARIABLES

    /** Frequency for idle breathing sway in cycles/second. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "0.0", UIMax = "5.0"))
    float BreathingFrequency = 0.4f;

    /** Vertical amplitude for idle breathing sway in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "0.0", UIMax = "5.0"))
    float BreathingAmplitudeV = 5.0f;

    /** Lateral amplitude for idle breathing sway in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "0.0", UIMax = "3.0"))
    float BreathingAmplitudeH = 3.0f;

    /** Roll amplitude for idle breathing sway in degrees. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "0.0", UIMax = "5.0"))
    float BreathingRollAmplitude = 3.0f;

    /** Frequency multiplier for breathing sway while sprinting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "1.0", UIMax = "8.0"))
    float BreathingSprintFrequencyMultiplier = 5.0f;

    /** Vertical amplitude multiplier for breathing sway while sprinting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "1.0", UIMax = "15.0"))
    float BreathingSprintAmplitudeMultiplier = 18.0f;

    /** Roll amplitude multiplier for breathing sway while sprinting. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "1.0", UIMax = "10.0"))
    float BreathingSprintRollMultiplier = 12.0f;

    /** Interpolation speed for breathing sway changes (idle to sprint and vice versa). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Breathing", meta = (ClampMin = "0.1", UIMax = "10.0"))
    float BreathingInterpSpeed = 2.0f;

#pragma endregion

#pragma region DYNAMIC_FOV_VARIABLES

    /** Base FOV used while walking or crouching. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|FOV",meta = (ClampMin = "40.0", ClampMax = "120.0",ToolTip = "Base FOV used while walking."))
    float BaseFOV = 70.0f;

    /** FOV used while sprinting. A slight push-out reinforces speed without being jarring. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|FOV",meta = (ClampMin = "40.0", ClampMax = "120.0",ToolTip = "FOV used while sprinting."))
    float SprintFOV = 85.0f;

    /** Interpolation speed for FOV transitions. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|FOV",meta = (ClampMin = "0.1", UIMax = "20.0",ToolTip = "Interp speed for FOV changes."))
    float FOVInterpSpeed = 3.0f;

#pragma endregion

#pragma region COMPONENTS
    /** First person mesh (arms) visible only to the owning player. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true", ToolTip = "First person mesh (arms) visible only to the owning player."))
    TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

    /** First person camera component used for first-person view. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true", ToolTip = "First person camera component used for first-person view."))
    TObjectPtr<UCameraComponent> FirstPersonCameraComponent;
#pragma endregion

#pragma region INPUTS
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used for movement on X/Y plane."))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used for look/aim on gamepad or virtual joystick."))
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used for look/aim with the mouse."))
    TObjectPtr<UInputAction> MouseLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used for jump start and end."))
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used for sprinting (run while held)."))
    TObjectPtr<UInputAction> RunAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "Input action used to toggle crouch on or off."))
    TObjectPtr<UInputAction> CrouchAction;
#pragma endregion

private:

    float HeadBobTime = 0.0f;
    float HeadBobAlpha = 0.0f;
    float BreathingTime = 0.0f;
    float BreathingAlpha = 0.0f;
    float SprintBreathAlpha = 0.0f;
    float PreviousBobSin = 0.0f;

    FVector  CameraBaseRelativeLocation = FVector::ZeroVector;
    FRotator CameraBaseRelativeRotation = FRotator::ZeroRotator;

    float CurrentHeadBobAmplitudeV = 6.0f;
    float CurrentHeadBobAmplitudeH = 4.0f;
    float CurrentBreathingFrequency  = 0.35f;
    float CurrentBreathingAmplitudeV = 5.0f;
    float CurrentBreathingAmplitudeH = 3.0f;
    float CurrentBreathingRoll       = 0.0f;

    FRotator CurrentCameraRollPitch = FRotator::ZeroRotator;

    FVector  CurrentBobTranslation = FVector::ZeroVector;
    FRotator CurrentBobRotation    = FRotator::ZeroRotator;

    FVector  CurrentBreathTranslation = FVector::ZeroVector;
    float    CurrentBreathRollValue   = 0.0f;
};