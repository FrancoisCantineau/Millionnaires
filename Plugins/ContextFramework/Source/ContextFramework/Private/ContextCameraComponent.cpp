#include "ContextCameraComponent.h"

#include "Data/ContextDataAsset.h"
#include "Data/ContextCameraSetupDataAsset.h"
#include "ContextComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/ContextStructData.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

UContextCameraComponent::UContextCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UContextCameraComponent::ConsumeLookInput(FVector2D Value)
{
	if (!Camera || !CurrentCameraData)
	{
		return;
	}

	const float MaxYaw   = CurrentCameraData->NeckYawLimit;
	const float MaxPitch = CurrentCameraData->NeckPitchLimit;
	const float Responsiveness = CurrentCameraData->NeckResponsiveness;
	const float DeadZone       = CurrentCameraData->NeckDeadZone;

	float YawInput   = Value.X * InputSensitivity;
	float PitchInput = Value.Y * InputSensitivity;

	if (FMath::Abs(YawInput) < DeadZone)   YawInput = 0.f;
	if (FMath::Abs(PitchInput) < DeadZone) PitchInput = 0.f;

	const float YawRatio   = FMath::Abs(LookOffset.Yaw)   / MaxYaw;
	const float PitchRatio = FMath::Abs(LookOffset.Pitch) / MaxPitch;

	const float YawResistance   = FMath::InterpEaseOut(1.f, 0.25f, YawRatio,   Responsiveness);
	const float PitchResistance = FMath::InterpEaseOut(1.f, 0.25f, PitchRatio, Responsiveness);

	YawInput   *= YawResistance;
	PitchInput *= PitchResistance;

	LookOffset.Yaw   += YawInput;
	LookOffset.Pitch += PitchInput;

	LookOffset.Yaw   = FMath::Clamp(LookOffset.Yaw,   -MaxYaw,   MaxYaw);
	LookOffset.Pitch = FMath::Clamp(LookOffset.Pitch, -MaxPitch, MaxPitch);
}

void UContextCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();

	UContextComponent* Contexts =
		GetOwner()->FindComponentByClass<UContextComponent>();

	if (!Contexts)
	{
		return;
	}

	Contexts->OnContextAdded.AddUObject(
		this,
		&ThisClass::HandleContextAdded);

	Contexts->OnContextRemoved.AddUObject(
		this,
		&ThisClass::HandleContextRemoved);
}

void UContextCameraComponent::HandleContextAdded(const FActiveContext& Context)
{
	if (!Camera) return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;

	if (!Context.Definition) return;
	if (!Context.Definition->CameraSetupData) return;

	CurrentCameraData = Context.Definition->CameraSetupData;
	CachedFocusTarget = Context.LookTarget;

	// Fallback : direction actuelle, roll forcé à 0 (caméra "droite")
	FRotator CurrentWorldRot = Camera->GetComponentRotation();
	BaseWorldRotation = FRotator(CurrentWorldRot.Pitch, CurrentWorldRot.Yaw, 0.f);

	LookOffset = FRotator::ZeroRotator;

	Camera->bUsePawnControlRotation = false;
	Character->bUseControllerRotationYaw = false;

	InputReceiver = true;
	bContextActive = true;
}

void UContextCameraComponent::HandleContextRemoved(const FActiveContext& Context)
{
	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	// restore rotation behavior
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		Character->bUseControllerRotationYaw = true;
	}

	ResetCameraConstraints(PC);

	ClearFocus();

	CurrentCameraData = nullptr;
	CachedFocusTarget = nullptr;
	bContextActive = false;

	if (Camera)
	{
		Camera->bUsePawnControlRotation = true;
	}
}

void UContextCameraComponent::ApplyCameraConstraints(
	APlayerController* PC,
	const UContextCameraSetupDataAsset* Data)
{
	if (!PC || !Data)
	{
		return;
	}

	PC->PlayerCameraManager->ViewYawMin   = BaseContextYaw + Data->CameraYawMin;
	PC->PlayerCameraManager->ViewYawMax   = BaseContextYaw + Data->CameraYawMax;
	PC->PlayerCameraManager->ViewPitchMin = Data->CameraPitchMin;
	PC->PlayerCameraManager->ViewPitchMax = Data->CameraPitchMax;
}

void UContextCameraComponent::ResetCameraConstraints(APlayerController* PC)
{
	if (!PC)
	{
		return;
	}

	PC->PlayerCameraManager->ViewYawMin   = -180.f;
	PC->PlayerCameraManager->ViewYawMax   =  180.f;
	PC->PlayerCameraManager->ViewPitchMin = -89.f;
	PC->PlayerCameraManager->ViewPitchMax =  89.f;
}

APlayerController* UContextCameraComponent::GetPlayerController() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return nullptr;
	}

	return Cast<APlayerController>(Pawn->GetController());
}

// ======================================================
// 🎯 UPDATE (IMPORTANT : effet visible du FocusPoint)
// ======================================================

void UContextCameraComponent::UpdateFocus(APlayerController* PC)
{
	if (!bHasFocus || !PC || !PC->GetPawn())
	{
		return;
	}

	FVector PawnLoc = PC->GetPawn()->GetActorLocation();

	FVector Direction = (FocusPoint - PawnLoc);
	if (Direction.IsNearlyZero())
	{
		return;
	}

	Direction.Normalize();

	FRotator TargetRot = Direction.Rotation();
	FRotator CurrentRot = PC->GetControlRotation();

	FRotator NewRot = FMath::RInterpTo(
		CurrentRot,
		TargetRot,
		GetWorld()->GetDeltaSeconds(),
		6.f
	);

	PC->SetControlRotation(NewRot);
}

// ======================================================
// 🎯 TICK : calcule la rotation cible en WORLD SPACE propre,
// puis la convertit en rotation RELATIVE au bone "head"
// (la caméra reste attachée, sa position suit le bone normalement)
// ======================================================

void UContextCameraComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Camera || !bContextActive)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;

	FVector HeadLocation = Mesh->GetBoneLocation(TEXT("head"));

	// --- Cible en world space propre (roll = 0 toujours) ---
	FRotator BaseRot = BaseWorldRotation;

	if (CachedFocusTarget.IsValid())
	{
		FVector ToFocus = (CachedFocusTarget->GetComponentLocation() - HeadLocation).GetSafeNormal();

		if (!ToFocus.IsNearlyZero())
		{
			BaseRot = ToFocus.Rotation();
			BaseRot.Roll = 0.f;
		}
	}

	FRotator TargetWorldRotation = BaseRot + LookOffset; // yaw/pitch monde, axes propres

	FQuat TargetWorldQuat  = TargetWorldRotation.Quaternion();
	FQuat CurrentWorldQuat = Camera->GetComponentQuat();

	float Alpha = FMath::Clamp(DeltaTime * 8.f, 0.f, 1.f);

	FQuat SmoothWorldQuat = FQuat::Slerp(CurrentWorldQuat, TargetWorldQuat, Alpha);
	SmoothWorldQuat.Normalize();

	// --- Convertit la cible world en rotation RELATIVE au bone head ---
	FQuat HeadWorldQuat = Mesh->GetBoneQuaternion(TEXT("head"));
	FQuat NewRelativeQuat = HeadWorldQuat.Inverse() * SmoothWorldQuat;
	NewRelativeQuat.Normalize();

	Camera->SetRelativeRotation(NewRelativeQuat.Rotator());
}