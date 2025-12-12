/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "PlayerCharacter" - Source
 * Notes: Playable first-person character derived from BaseCharacter. Handles camera, input and applying character definitions.
 */

#include "Characters/Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Millionnaires.h"
#include "Systems/CharacterSelectionSubsystem.h"
#include "Components/Characters/CharacterStatsComponent.h"
#include "Data/CharacterDefinition.h"
#include "Engine/GameInstance.h"
#include "Animation/AnimInstance.h"

#pragma region INIT

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Capsule setup
    GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

    // --- First person mesh ---
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetupAttachment(GetMesh());
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
    FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

    // --- First person camera ---
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
    FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;
    FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
    FirstPersonCameraComponent->bEnableFirstPersonScale = true;
    FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
    FirstPersonCameraComponent->FirstPersonScale = 0.6f;

    // Hide 3rd person mesh for owning player
    GetMesh()->SetOwnerNoSee(true);
    GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

    // Movement tuning
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->BrakingDecelerationFalling = 1500.0f;
        MoveComp->AirControl = 0.5f;
        MoveComp->NavAgentProps.bCanCrouch = true;

        // Initialise runtime movement speeds
        CurrentMaxWalkSpeed = WalkSpeed;
        DesiredMaxWalkSpeed = WalkSpeed;
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }

    bUseControllerRotationYaw = true;
    PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    UCharacterDefinition* SelectedDefinition = nullptr;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UCharacterSelectionSubsystem* SelectionSubsystem = GameInstance->GetSubsystem<UCharacterSelectionSubsystem>())
        {
            SelectedDefinition = SelectionSubsystem->GetSelectedCharacter();
        }
    }

    if (SelectedDefinition)
    {
        ApplyCharacterDefinition(SelectedDefinition);
    }
    else if (UCharacterStatsComponent* Stats = GetStatsComponent())
    {
        if (Stats->GetCharacterDefinition())
        {
            ApplyCharacterDefinition(Stats->GetCharacterDefinition());
        }
    }

    // Ensure the desired movement speed matches the initial state.
    UpdateMovementSpeed();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SmoothUpdateMovementSpeed(DeltaSeconds);
}

#pragma endregion

#pragma region INPUT_BINDING

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::HandleMove);
        }

        if (LookAction)
        {
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::HandleLook);
        }

        if (MouseLookAction)
        {
            EnhancedInput->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::HandleLook);
        }

        if (JumpAction)
        {
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::HandleJumpStart);
            EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::HandleJumpEnd);
        }

        if (RunAction)
        {
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::HandleRunStart);
            EnhancedInput->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::HandleRunEnd);
        }

        if (CrouchAction)
        {
            EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::HandleCrouchToggle);
        }
    }
    else
    {
        UE_LOG(LogMillionnaires, Error, TEXT("'%s' failed to find an EnhancedInputComponent. This character expects Enhanced Input."), *GetNameSafe(this));
    }
}

#pragma endregion

#pragma region INPUT_HANDLERS

void APlayerCharacter::HandleMove(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    ApplyMovement(MovementVector.X, MovementVector.Y);
}

void APlayerCharacter::HandleLook(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();
    ApplyAim(LookVector.X, LookVector.Y);
}

void APlayerCharacter::HandleJumpStart()
{
    Jump();
}

void APlayerCharacter::HandleJumpEnd()
{
    StopJumping();
}

void APlayerCharacter::HandleRunStart()
{
    // Running is only considered when not crouched.
    bIsSprinting = true;
    UpdateMovementSpeed();

    if (bDebugMovement)
    {
        const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
        const float Speed = MoveComp ? MoveComp->MaxWalkSpeed : -1.0f;
        UE_LOG(LogMillionnaires, Log, TEXT("[Player] Sprint started. DesiredMaxWalkSpeed=%.1f CurrentMaxWalkSpeed=%.1f"),
            DesiredMaxWalkSpeed, Speed);
    }
}

void APlayerCharacter::HandleRunEnd()
{
    bIsSprinting = false;
    UpdateMovementSpeed();

    if (bDebugMovement)
    {
        const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
        const float Speed = MoveComp ? MoveComp->MaxWalkSpeed : -1.0f;
        UE_LOG(LogMillionnaires, Log, TEXT("[Player] Sprint ended. DesiredMaxWalkSpeed=%.1f CurrentMaxWalkSpeed=%.1f"),
            DesiredMaxWalkSpeed, Speed);
    }
}

void APlayerCharacter::HandleCrouchToggle()
{
    if (IsCrouched())
    {
        UnCrouch();
    }
    else
    {
        Crouch();

        // Optionally cancel sprint when crouching to avoid conflicting states.
        if (bIsSprinting)
        {
            bIsSprinting = false;
        }
    }

    UpdateMovementSpeed();

    if (bDebugMovement)
    {
        const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
        const float Speed = MoveComp ? MoveComp->MaxWalkSpeed : -1.0f;
        UE_LOG(LogMillionnaires, Log, TEXT("[Player] Crouch toggled. IsCrouched=%s DesiredMaxWalkSpeed=%.1f CurrentMaxWalkSpeed=%.1f"),
            IsCrouched() ? TEXT("true") : TEXT("false"),
            DesiredMaxWalkSpeed,
            Speed);
    }
}

#pragma endregion

#pragma region MOVEMENT_INTERNAL

void APlayerCharacter::UpdateMovementSpeed()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp)
    {
        return;
    }

    // Base = walk speed
    float TargetSpeed = WalkSpeed;

    // Sprint only when not crouched
    if (bIsSprinting && !MoveComp->IsCrouching())
    {
        TargetSpeed = RunSpeed;
    }

    // Crouch always forces crouch speed (slower than walk/run)
    if (MoveComp->IsCrouching())
    {
        TargetSpeed = CrouchSpeed;
    }

    DesiredMaxWalkSpeed = TargetSpeed;

    // If there is no ramp time, apply instant change
    if (TimeToReachMaxSpeed <= 0.0f)
    {
        CurrentMaxWalkSpeed = DesiredMaxWalkSpeed;
        MoveComp->MaxWalkSpeed = DesiredMaxWalkSpeed;
    }
}

void APlayerCharacter::SmoothUpdateMovementSpeed(float DeltaSeconds)
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp || TimeToReachMaxSpeed <= 0.0f)
    {
        // Already handled in UpdateMovementSpeed when TimeToReachMaxSpeed <= 0
        return;
    }

    const float DeltaToTarget = DesiredMaxWalkSpeed - CurrentMaxWalkSpeed;
    if (FMath::IsNearlyZero(DeltaToTarget))
    {
        return;
    }

    const float DeltaSpeedPerSecond = DeltaToTarget / TimeToReachMaxSpeed;
    const float Step = DeltaSpeedPerSecond * DeltaSeconds;
    float NewSpeed = CurrentMaxWalkSpeed + Step;

    // Clamp to avoid overshooting the target
    if ((DeltaToTarget > 0.0f && NewSpeed > DesiredMaxWalkSpeed) ||
        (DeltaToTarget < 0.0f && NewSpeed < DesiredMaxWalkSpeed))
    {
        NewSpeed = DesiredMaxWalkSpeed;
    }

    CurrentMaxWalkSpeed = NewSpeed;
    MoveComp->MaxWalkSpeed = NewSpeed;
}

void APlayerCharacter::ApplyMovement(float RightInput, float ForwardInput)
{
    if (!Controller)
    {
        return;
    }

    AddMovementInput(GetActorRightVector(), RightInput);
    AddMovementInput(GetActorForwardVector(), ForwardInput);
}

void APlayerCharacter::ApplyAim(float YawInput, float PitchInput)
{
    if (!Controller)
    {
        return;
    }

    AddControllerYawInput(YawInput);
    AddControllerPitchInput(PitchInput);
}

#pragma endregion

#pragma region CHARACTER_DEFINITION

void APlayerCharacter::ApplyCharacterDefinition(UCharacterDefinition* Definition)
{
    if (!Definition)
    {
        return;
    }

    if (UCharacterStatsComponent* Stats = GetStatsComponent())
    {
        Stats->InitializeFromDefinition(Definition);
    }

    ApplyVisualsFromDefinition(Definition);
}

void APlayerCharacter::ApplyVisualsFromDefinition(UCharacterDefinition* Definition)
{
    if (!Definition)
    {
        return;
    }

    if (USkeletalMesh* NewWorldMesh = Definition->GetWorldMesh())
    {
        GetMesh()->SetSkeletalMesh(NewWorldMesh);
    }

    if (TSubclassOf<UAnimInstance> NewWorldAnimClass = Definition->GetWorldAnimClass())
    {
        GetMesh()->SetAnimInstanceClass(NewWorldAnimClass);
    }

    if (FirstPersonMesh)
    {
        if (USkeletalMesh* NewFirstPersonMesh = Definition->GetFirstPersonMesh())
        {
            FirstPersonMesh->SetSkeletalMesh(NewFirstPersonMesh);
        }

        if (TSubclassOf<UAnimInstance> NewFirstPersonAnimClass = Definition->GetFirstPersonAnimClass())
        {
            FirstPersonMesh->SetAnimInstanceClass(NewFirstPersonAnimClass);
        }
    }
}

#pragma endregion
