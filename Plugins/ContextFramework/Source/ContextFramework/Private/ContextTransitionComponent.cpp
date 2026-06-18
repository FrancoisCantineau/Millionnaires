#include "ContextTransitionComponent.h"

#include "ContextComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/ContextDataAsset.h"
#include "GameFramework/PlayerController.h"

UContextTransitionComponent::UContextTransitionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UContextTransitionComponent::BeginPlay()
{
	Super::BeginPlay();

	ContextComponent = GetOwner()->FindComponentByClass<UContextComponent>();

	if (!ContextComponent)
	{
		return;
	}

	ContextComponent->OnContextAdded.AddUObject(
		this,
		&ThisClass::HandleContextAdded);

	ContextComponent->OnContextRemoved.AddUObject(
		this,
		&ThisClass::HandleContextRemoved);
}

void UContextTransitionComponent::HandleContextAdded(const FActiveContext& Context)
{
	CurrentContext = Context;

	if (ContextComponent)
	{
		ContextComponent->SetContextState(EContextState::Transitioning);
	}
	
	StartTransition(Context);
}

void UContextTransitionComponent::HandleContextRemoved(const FActiveContext& Context)
{
	if (APlayerController* PC = GetPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			PC->SetViewTarget(Pawn);
		}
	}
}

void UContextTransitionComponent::StartTransition(const FActiveContext& Context)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Context.SnapTarget)
	{
		return;
	}

	// disable movement during transition
	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->DisableMovement();
	}

	StartTransform = Character->GetActorTransform();

	// ✅ CLEAN SNAP TARGET (YAW ONLY)
	FVector TargetLoc = Context.SnapTarget->GetComponentLocation();

	FVector Forward = Context.SnapTarget->GetForwardVector();
	Forward.Z = 0.f;
	Forward.Normalize();

	FRotator TargetRot = Forward.Rotation();

	TargetTransform = FTransform(TargetRot, TargetLoc);

	TransitionAlpha = 0.f;
	bIsTransitioning = true;
}

void UContextTransitionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsTransitioning)
	{
		return;
	}

	TickTransition(DeltaTime);
}

void UContextTransitionComponent::TickTransition(float DeltaTime)
{
	TransitionAlpha += DeltaTime / TransitionDuration;

	if (TransitionAlpha >= 1.f)
	{
		TransitionAlpha = 1.f;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	// Smooth blend transform
	FTransform NewTransform;
	NewTransform.Blend(StartTransform, TargetTransform, TransitionAlpha);
	
	Character->SetActorLocationAndRotation(
	NewTransform.GetLocation(),
	FRotator(0.f, NewTransform.Rotator().Yaw, 0.f)
);	

	AController* Controller = Character->GetController();
	if (Controller)
	{
		FRotator ControlRot = Controller->GetControlRotation();
		ControlRot.Yaw = NewTransform.Rotator().Yaw;
		Controller->SetControlRotation(ControlRot);
	}

	// finish
	if (TransitionAlpha >= 1.f)
	{
		bIsTransitioning = false;
		OnTransitionApproachFinished();
	}
}

void UContextTransitionComponent::OnTransitionApproachFinished()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		if (Character->GetCharacterMovement())
		{
		//	Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}

	PlayEnterMontage(CurrentContext);
}

void UContextTransitionComponent::EndTransition()
{
	if (ContextComponent)
	{
		ContextComponent->SetContextState(EContextState::Active);
	}
}

void UContextTransitionComponent::ApplySnapTarget(const FActiveContext& Context)
{
	// not used anymore (kept for future extension)
}

void UContextTransitionComponent::ApplyViewTarget(const FActiveContext& Context)
{
	if (!Context.ViewTarget)
	{
		return;
	}

	if (APlayerController* PC = GetPlayerController())
	{
		PC->SetViewTarget(Context.ViewTarget);
	}
}

APlayerController* UContextTransitionComponent::GetPlayerController() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return nullptr;
	}

	return Cast<APlayerController>(Pawn->GetController());
}

void UContextTransitionComponent::PlayEnterMontage(const FActiveContext& Context)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
	UAnimMontage* Montage = Context.Definition->EnterMontage;
	
	if (!Context.Definition || !Character || !Context.Definition->EnterMontage || !Anim || !Montage)
	{
		EndTransition();
		return;
	}
	
	Anim->Montage_Play(Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UContextTransitionComponent::OnEnterMontageEnded);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UContextTransitionComponent::OnEnterMontageEnded(UAnimMontage*, bool bInterrupted)
{
	EndTransition();
}