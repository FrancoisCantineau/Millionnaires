/*
 * Millionaire Project, 2025
 * Created by: "0nnen"
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

        MoveComp->MaxAcceleration = 800.0f;
        MoveComp->BrakingDecelerationWalking = 600.0f;
        MoveComp->GroundFriction = 4.0f;

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

    if (FirstPersonCameraComponent)
    {
        CameraBaseRelativeLocation = FirstPersonCameraComponent->GetRelativeLocation();
        CameraBaseRelativeRotation = FirstPersonCameraComponent->GetRelativeRotation();
        FirstPersonCameraComponent->FirstPersonFieldOfView = BaseFOV;
    }

    HeadBobFrequency                  = 8.0f;
    HeadBobAmplitudeV                 = 4.5f;
    HeadBobAmplitudeH                 = 1.5f;
    HeadBobSprintFrequencyMultiplier  = 1.2f;
    HeadBobSprintVerticalMultiplier   = 2.2f;
    HeadBobSprintLateralMultiplier    = 1.0f;
    HeadBobRollAmplitude              = 2.5f;
    HeadBobPitchAmplitude             = 1.5f;
    HeadBobRotationInterpSpeed        = 10.0f;
    HeadBobInterpSpeed                = 6.0f;

    BreathingFrequency                = 0.65f;
    BreathingAmplitudeV               = 18.0f;
    BreathingAmplitudeH               = 0.0f;
    BreathingRollAmplitude            = 1.5f;
    BreathingSprintFrequencyMultiplier= 2.5f;
    BreathingSprintAmplitudeMultiplier= 0.9f;
    BreathingSprintRollMultiplier     = 2.0f;
    BreathingInterpSpeed              = 1.5f;

    SprintFOV     = 85.0f;
    FOVInterpSpeed= 3.0f;


    CurrentHeadBobAmplitudeV   = HeadBobAmplitudeV;
    CurrentHeadBobAmplitudeH   = HeadBobAmplitudeH;
    CurrentBreathingFrequency  = BreathingFrequency;
    CurrentBreathingAmplitudeV = BreathingAmplitudeV;
    CurrentBreathingAmplitudeH = BreathingAmplitudeH;
    CurrentBreathingRoll       = 0.0f;

    UCharacterDefinition* SelectedDefinition = nullptr;
    if (!SelectedDefinition)
    {
        if (StatsComponent)
        {
            if (StatsComponent->GetCharacterDefinition())
            {
                SelectedDefinition = StatsComponent->GetCharacterDefinition();
            }
        }
    }

    if (SelectedDefinition)
    {
        ApplyCharacterDefinition(SelectedDefinition);
    }


    // Ensure the desired movement speed matches the initial state.
    UpdateMovementSpeed();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SmoothUpdateMovementSpeed(DeltaSeconds);
    UpdateCameraFeel(DeltaSeconds);
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
        UE_LOG(LogMillionnaires, Log, TEXT("[Player] Sprint ended."));
    }
}

void APlayerCharacter::HandleCrouchToggle()
{
    if (bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }

    UpdateMovementSpeed();

    if (bDebugMovement)
    {
        UE_LOG(LogMillionnaires, Log, TEXT("[Player] Crouch toggled — IsCrouched=%s"), bIsCrouched ? TEXT("true") : TEXT("false"));
    }
}

#pragma endregion

#pragma region MOVEMENT_INTERNAL

void APlayerCharacter::UpdateMovementSpeed()
{
    if (bIsCrouched)
    {
        DesiredMaxWalkSpeed = CrouchSpeed;
    }
    else if (bIsSprinting)
    {
        DesiredMaxWalkSpeed = RunSpeed;
    }
    else
    {
        DesiredMaxWalkSpeed = WalkSpeed;
    }
}

void APlayerCharacter::SmoothUpdateMovementSpeed(float DeltaSeconds)
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp)
    {
        return;
    }

    if (TimeToReachMaxSpeed <= 0.0f)
    {
        CurrentMaxWalkSpeed = DesiredMaxWalkSpeed;
        MoveComp->MaxWalkSpeed = DesiredMaxWalkSpeed;
        return;
    }

    const float DeltaToTarget = DesiredMaxWalkSpeed - CurrentMaxWalkSpeed;
    if (FMath::IsNearlyZero(DeltaToTarget))
    {
        return;
    }

    const float DeltaSpeedPerSecond = DeltaToTarget / TimeToReachMaxSpeed;
    float NewSpeed = CurrentMaxWalkSpeed + DeltaSpeedPerSecond * DeltaSeconds;

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

    if (StatsComponent)
    {
        StatsComponent->InitializeFromDefinition(Definition);
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

#pragma region CAMERA_FEEL

void APlayerCharacter::UpdateCameraFeel(float DeltaSeconds)
{
    if (!FirstPersonCameraComponent)
    {
        return;
    }

    UpdateBreathingSway(DeltaSeconds);
    UpdateHeadBob(DeltaSeconds);
    UpdateDynamicFOV(DeltaSeconds);
}

void APlayerCharacter::UpdateHeadBob(float DeltaSeconds)
{
    const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp || !FirstPersonCameraComponent) return;

    const bool bMoving = MoveComp->IsMovingOnGround()
        && MoveComp->Velocity.SizeSquared2D() > 25.0f;

    const float TargetAlpha = bMoving ? 1.0f : 0.0f;
    HeadBobAlpha = FMath::FInterpTo(HeadBobAlpha, TargetAlpha, DeltaSeconds, HeadBobInterpSpeed);


    if (HeadBobAlpha < 0.001f)
    {
        HeadBobTime           = 0.0f;
        PreviousBobSin        = 0.0f;
        CurrentBobTranslation = FVector::ZeroVector;
        CurrentCameraRollPitch = FMath::RInterpTo(CurrentCameraRollPitch, FRotator::ZeroRotator, DeltaSeconds, HeadBobRotationInterpSpeed);
        CurrentBobRotation = CurrentCameraRollPitch;
        FirstPersonCameraComponent->SetRelativeLocation(CameraBaseRelativeLocation + CurrentBreathTranslation);
        FirstPersonCameraComponent->SetRelativeRotation(CameraBaseRelativeRotation + CurrentCameraRollPitch);
        return;
    }

    const float Freq = HeadBobFrequency * (bIsSprinting ? HeadBobSprintFrequencyMultiplier : 1.0f);
    HeadBobTime += DeltaSeconds * Freq;

    const float TargetAmpV = HeadBobAmplitudeV * (bIsSprinting ? HeadBobSprintVerticalMultiplier : 1.0f);
    const float TargetAmpH = HeadBobAmplitudeH * (bIsSprinting ? HeadBobSprintLateralMultiplier  : 1.0f);
    CurrentHeadBobAmplitudeV = FMath::FInterpTo(CurrentHeadBobAmplitudeV, TargetAmpV, DeltaSeconds, HeadBobInterpSpeed);
    CurrentHeadBobAmplitudeH = FMath::FInterpTo(CurrentHeadBobAmplitudeH, TargetAmpH, DeltaSeconds, HeadBobInterpSpeed);

    const float BobV = FMath::Sin(HeadBobTime) * CurrentHeadBobAmplitudeV * HeadBobAlpha;
    
    const float BobH = bIsSprinting
        ? FMath::Cos(HeadBobTime * 0.5f) * CurrentHeadBobAmplitudeH * HeadBobAlpha
        : FMath::Sin(HeadBobTime * 0.5f) * CurrentHeadBobAmplitudeH * HeadBobAlpha;

    const float BobFwd = FMath::Sin(HeadBobTime * 2.0f) * (bIsSprinting ? 4.0f : 2.0f) * HeadBobAlpha;

    CurrentBobTranslation = FVector(BobFwd, BobH, BobV);

    const float RollAmp = HeadBobRollAmplitude * (bIsSprinting ? HeadBobSprintLateralMultiplier : 1.0f);
    const float TargetRoll = bIsSprinting
        ? FMath::Cos(HeadBobTime * 0.5f) * RollAmp * HeadBobAlpha
        : FMath::Sin(HeadBobTime * 0.5f) * RollAmp * HeadBobAlpha;

    const float TargetPitch = -FMath::Abs(FMath::Sin(HeadBobTime)) * HeadBobPitchAmplitude * HeadBobAlpha;

    const FRotator TargetRot = FRotator(TargetPitch, 0.f, TargetRoll);
    CurrentCameraRollPitch = FMath::RInterpTo(
        CurrentCameraRollPitch, TargetRot, DeltaSeconds, HeadBobRotationInterpSpeed);
    CurrentBobRotation = CurrentCameraRollPitch;

    FirstPersonCameraComponent->SetRelativeLocation(
        CameraBaseRelativeLocation + CurrentBobTranslation + CurrentBreathTranslation);

    FirstPersonCameraComponent->SetRelativeRotation(CameraBaseRelativeRotation + CurrentBobRotation);

    PreviousBobSin = FMath::Sin(HeadBobTime);
}

void APlayerCharacter::UpdateBreathingSway(float DeltaSeconds)
{
    const float TargetIdle = 1.0f - HeadBobAlpha;
    BreathingAlpha = FMath::FInterpTo(BreathingAlpha, TargetIdle, DeltaSeconds, BreathingInterpSpeed);

    const float TargetSprint = bIsSprinting ? 1.0f : 0.0f;
    SprintBreathAlpha = FMath::FInterpTo(SprintBreathAlpha, TargetSprint, DeltaSeconds, BreathingInterpSpeed);

    const float ActiveAlpha = FMath::Max(BreathingAlpha, SprintBreathAlpha);

    if (ActiveAlpha < 0.001f)
    {
        CurrentBreathTranslation = FVector::ZeroVector;
        CurrentBreathRollValue   = 0.0f;
        return;
    }

    const float TargetFreq = BreathingFrequency  * (bIsSprinting ? BreathingSprintFrequencyMultiplier : 1.0f);
    const float TargetAmpV = BreathingAmplitudeV * (bIsSprinting ? BreathingSprintAmplitudeMultiplier : 1.0f);
    const float TargetAmpH = 0.0f;
    const float TargetRoll = BreathingRollAmplitude * (bIsSprinting ? BreathingSprintRollMultiplier : 1.0f);

    CurrentBreathingFrequency  = FMath::FInterpTo(CurrentBreathingFrequency,  TargetFreq, DeltaSeconds, BreathingInterpSpeed);
    CurrentBreathingAmplitudeV = FMath::FInterpTo(CurrentBreathingAmplitudeV, TargetAmpV, DeltaSeconds, BreathingInterpSpeed);
    CurrentBreathingAmplitudeH = FMath::FInterpTo(CurrentBreathingAmplitudeH, TargetAmpH, DeltaSeconds, BreathingInterpSpeed);
    CurrentBreathingRoll       = FMath::FInterpTo(CurrentBreathingRoll,       TargetRoll, DeltaSeconds, BreathingInterpSpeed);

    BreathingTime += DeltaSeconds * CurrentBreathingFrequency;

    const float SwayV = FMath::Sin(BreathingTime) * CurrentBreathingAmplitudeV * ActiveAlpha;
    const float FwdAmp = bIsSprinting ? 3.5f : 3.0f;
    const float SwayFwd = FMath::Sin(BreathingTime * 2.0f) * FwdAmp * ActiveAlpha;

    CurrentBreathTranslation = FVector(SwayFwd, 0.f, SwayV);
    CurrentBreathRollValue   = FMath::Cos(BreathingTime) * CurrentBreathingRoll * ActiveAlpha;
}

void APlayerCharacter::UpdateDynamicFOV(float DeltaSeconds)
{
    const float TargetFOV = bIsSprinting ? SprintFOV : BaseFOV;

    const float CurrentFOV = FirstPersonCameraComponent->FirstPersonFieldOfView;
    const float NewFOV     = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, FOVInterpSpeed);

    FirstPersonCameraComponent->FirstPersonFieldOfView = NewFOV;
}


#pragma endregion