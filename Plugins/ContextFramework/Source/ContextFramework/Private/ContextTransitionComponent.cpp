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
	
	if (!Context.SnapTarget && (!Context.Definition || !Context.Definition->EnterMontage))
	{
		if (ContextComponent)
			ContextComponent->SetContextState(EContextState::Active);
		return;
	}
 
	if (ContextComponent)
		ContextComponent->SetContextState(EContextState::Transitioning);
 
	StartTransition(Context);
}
 
void UContextTransitionComponent::HandleContextRemoved(const FActiveContext& Context)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
 
void UContextTransitionComponent::StartTransition(const FActiveContext& Context)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Context.SnapTarget)
	{
		return;
	}
	
	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->DisableMovement();
	}
 
	StartTransform = Character->GetActorTransform();
	
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
	if (!CurrentContext.Definition) return;
 
	if (CurrentContext.Definition->LifetimePolicy == EContextLifetimePolicy::RemoveOnEnterEnd)
	{
		ContextComponent->RemoveContext(CurrentContext.Handle);
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

	if (!Character)
	{
		if (ContextComponent)
		{
			ContextComponent->SetContextState(EContextState::Active);
		}

		EndTransition();
		return;
	}

	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();

	UAnimMontage* Montage = Context.TransitionData.EnterMontage;
	
	if (!Montage && Context.Definition)
	{
		Montage = Context.Definition->EnterMontage;
	}

	if (!Anim || !Montage)
	{
		if (ContextComponent)
		{
			ContextComponent->SetContextState(EContextState::Active);
		}

		EndTransition();
		return;
	}
 
	Anim->Montage_Play(Montage);

	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &UContextTransitionComponent::OnEnterMontageBlendingOut);
	Anim->Montage_SetBlendingOutDelegate(BlendOutDelegate, Montage);
 

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UContextTransitionComponent::OnEnterMontageEnded);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);
}
 
void UContextTransitionComponent::OnEnterMontageBlendingOut(UAnimMontage*, bool bInterrupted)
{
	if (ContextComponent)
	{
		ContextComponent->SetContextState(EContextState::Active);
	}
}
 
void UContextTransitionComponent::OnEnterMontageEnded(UAnimMontage*, bool bInterrupted)
{
	EndTransition();
}
 
void UContextTransitionComponent::RequestContextExit(FContextHandle Handle, UAnimMontage* ExitMontage)
{
	const FActiveContext* Context = ContextComponent->GetTopContext();
	if (!Context)
		return;
 
	PendingExitHandle = Handle;
 
	UAnimMontage* FinalExitMontage = ExitMontage;
 
	if (!FinalExitMontage && Context->Definition)
	{
		FinalExitMontage = Context->Definition->EndMontage;
	}
 
	if (!FinalExitMontage)
	{
		ContextComponent->RemoveContext(Handle);
		return;
	}
 
	ContextComponent->SetContextState(
		EContextState::Transitioning
	);
 
	PlayExitMontage(FinalExitMontage);
}
 
void UContextTransitionComponent::PlayExitMontage(UAnimMontage* Montage)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Montage)
	{
		ContextComponent->RemoveContext(PendingExitHandle);
		return;
	}
 
	UAnimInstance* Anim = Character->GetMesh()->GetAnimInstance();
	if (!Anim)
	{
		ContextComponent->RemoveContext(PendingExitHandle);
		return;
	}
	ContextComponent->SetContextState(EContextState::StartExit);
	Anim->Montage_Play(Montage);
	
	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &UContextTransitionComponent::OnExitMontageBlendingOut);
	Anim->Montage_SetBlendingOutDelegate(BlendOutDelegate, Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UContextTransitionComponent::OnExitMontageEnded);
	Anim->Montage_SetEndDelegate(EndDelegate, Montage);
}
 
void UContextTransitionComponent::OnExitMontageBlendingOut(UAnimMontage*, bool bInterrupted)
{
	ContextComponent->SetContextState(EContextState::Exiting);
}

void UContextTransitionComponent::OnExitMontageEnded(UAnimMontage*, bool bInterrupted)
{
	ContextComponent->RemoveContext(PendingExitHandle);
	PendingExitHandle = FContextHandle();
}