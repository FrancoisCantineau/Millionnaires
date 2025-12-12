/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
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

protected:

#pragma region MOVEMENTS_VARIABLES
    /** Walking speed used when the player is moving normally. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Walking speed used when the player is moving normally."))
    float WalkSpeed = 350.0f;

    /** Running speed used while the sprint input is held. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Running speed used while the sprint input is held."))
    float RunSpeed = 650.0f;

    /** Movement speed used while crouched. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ToolTip = "Movement speed used while crouched."))
    float CrouchSpeed = 250.0f;

    /** Time in seconds needed to reach the desired max speed from the current speed. 0 = instant change. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Acceleration", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "3.0", ToolTip = "Time in seconds needed to reach the desired max speed from the current speed. 0 = instant change."))
    float TimeToReachMaxSpeed = 0.25f;

    /** Enable or disable debug logs for movement state changes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "If true, the player will log movement state changes (walk / run / crouch)."))
    bool bDebugMovement = false;

    /** True while sprint input is held. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "True while sprint input is held."))
    bool bIsSprinting = false;

    /** Desired target max walk speed computed from movement state. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "Desired target max walk speed computed from movement state."))
    float DesiredMaxWalkSpeed = 350.0f;

    /** Current interpolated max walk speed actually applied to CharacterMovementComponent. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Runtime", meta = (ToolTip = "Current interpolated max walk speed actually applied to CharacterMovementComponent."))
    float CurrentMaxWalkSpeed = 350.0f;
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
};
