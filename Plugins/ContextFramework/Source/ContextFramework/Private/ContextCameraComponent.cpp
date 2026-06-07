// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextCameraComponent.h"
#include "Data/ContextDataAsset.h"
#include "Data/ContextCameraSetupDataAsset.h"
#include "ContextComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/ContextStructData.h"

// Sets default values for this component's properties
UContextCameraComponent::UContextCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UContextCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	UContextComponent* Contexts =
		GetOwner()->FindComponentByClass<UContextComponent>();

	if (!Contexts)
	{
		return;
	}

	Contexts->OnContextAdded.AddUObject(
		this,
		&UContextCameraComponent::HandleContextAdded);

	Contexts->OnContextRemoved.AddUObject(
		this,
		&UContextCameraComponent::HandleContextRemoved);
	
}

void UContextCameraComponent::HandleContextRemoved(const FActiveContext& Context)
{
	APlayerController* PC = GetPlayerController();

	if (!PC)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(GetOwner());

	if (Pawn)
	{
		PC->SetViewTargetWithBlend(
			Context.ViewTarget,
			0.3f);
	}

	PC->PlayerCameraManager->ViewYawMin = -180.f;
	PC->PlayerCameraManager->ViewYawMax = 180.f;

	PC->PlayerCameraManager->ViewPitchMin = -89.f;
	PC->PlayerCameraManager->ViewPitchMax = 89.f;
}

void UContextCameraComponent::HandleContextAdded(const FActiveContext& Context)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !Context.Definition)
		return;

	const UContextCameraSetupDataAsset* CameraData =
		Context.Definition->CameraSetupData;

	if (!CameraData)
		return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
		return;

	UCameraComponent* Camera =
		Pawn->FindComponentByClass<UCameraComponent>();

	if (!Camera)
		return;

	// 1. GET TARGET
	const AActor* Target = Context.ViewTarget;
	if (!Target)
		return;

	// 2. CALCUL POSITION CAMERA
	const FVector CamLoc = Camera->GetComponentLocation();
	const FVector TargetLoc = Target->GetActorLocation();

	const FRotator LookAtRot = (TargetLoc - CamLoc).Rotation();

	// 3. LOCK INPUT (important)
	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);

	// 4. SNAP CAMERA DIRECTLY
	Camera->SetWorldLocation(TargetLoc);
	Camera->SetWorldRotation(LookAtRot);

	// 5. SET CONTROL ROTATION (sync UE pipeline)
	PC->SetControlRotation(LookAtRot);
	Pawn->FaceRotation(LookAtRot, 0.f);

	// 6. STORE BASE ROTATION FOR CONSTRAINTS
	BaseContextRotation = LookAtRot;

	// 7. APPLY CONSTRAINTS (RELATIVE, IMPORTANT)
	PC->PlayerCameraManager->ViewYawMin =
		BaseContextRotation.Yaw + CameraData->CameraYawMin;

	PC->PlayerCameraManager->ViewYawMax =
		BaseContextRotation.Yaw + CameraData->CameraYawMax;

	PC->PlayerCameraManager->ViewPitchMin =
		BaseContextRotation.Pitch + CameraData->CameraPitchMin;

	PC->PlayerCameraManager->ViewPitchMax =
		BaseContextRotation.Pitch + CameraData->CameraPitchMax;

	bContextActive = true;
}


// Called every frame
void UContextCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
}


APlayerController* UContextCameraComponent::GetPlayerController() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());

	if (!Pawn)
	{
		return nullptr;
	}

	return Cast<APlayerController>(
		Pawn->GetController());
}

