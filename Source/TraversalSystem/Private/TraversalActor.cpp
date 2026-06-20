#include "TraversalActor.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"

// ============================================================

ATraversalActor::ATraversalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	StartPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(RootComponent);

	EndPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(RootComponent);

	ExitPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitPoint"));
	ExitPoint->SetupAttachment(RootComponent);
}

// ============================================================
// INPUT ROUTING (NEW ARCHI)
// ============================================================

void ATraversalActor::HandleTraversalInput(
	UTraversalComponent* Component,
	const FVector2D& Input)
{
	if (FMath::Abs(Input.X) > FMath::Abs(Input.Y))
	{
		if (Input.X > 0)
			MoveRight(Component);
		else
			MoveLeft(Component);
	}
	else
	{
		if (Input.Y > 0)
			MoveForward(Component);
		else
			MoveBackward(Component);
	}
}

FVector ATraversalActor::GetTraversalDirection(const UTraversalComponent* Component)
{
	ACharacter* OwnerCharacter = Component ? Component->GetOwnerCharacter() : nullptr;

	switch (DirectionSource)
	{
	case ETraversalDirectionSource::FixedLocal:
		return GetActorTransform().TransformVectorNoScale(TraversalDirection.GetSafeNormal());

	case ETraversalDirectionSource::FixedWorld:
		return TraversalDirection.GetSafeNormal();

	case ETraversalDirectionSource::PlayerForward:
		return OwnerCharacter ? OwnerCharacter->GetActorForwardVector() : GetActorForwardVector();

	case ETraversalDirectionSource::PlayerUp:
		return OwnerCharacter ? OwnerCharacter->GetActorUpVector() : GetActorUpVector();

	case ETraversalDirectionSource::PlayerRight:
		return OwnerCharacter ? OwnerCharacter->GetActorRightVector() : GetActorRightVector();

	case ETraversalDirectionSource::Custom:
	default:
		return GetActorForwardVector();
	}
}

// DEFAULTS
void ATraversalActor::MoveForward(UTraversalComponent*) {}
void ATraversalActor::MoveBackward(UTraversalComponent*) {}
void ATraversalActor::MoveLeft(UTraversalComponent*) {}
void ATraversalActor::MoveRight(UTraversalComponent*) {}

// ============================================================

FTraversalEntryInfo ATraversalActor::GetEntryInfo(ACharacter* Character, UTraversalComponent* Component) const
{
	FTraversalEntryInfo Info;
	Info.EntryPoint = const_cast<ATraversalActor*>(this)->GetEntryPointForCharacter(Character);
	Info.EnterTransition = nullptr;
	return Info;
}

USceneComponent* ATraversalActor::GetEntryPointForCharacter(ACharacter* Character)
{
	FVector Loc = Character->GetActorLocation();

	float DistA = FVector::Dist(Loc, StartPoint->GetComponentLocation());
	float DistB = FVector::Dist(Loc, EndPoint->GetComponentLocation());

	return (DistA <= DistB) ? StartPoint : EndPoint;
}

UAnimMontage* ATraversalActor::GetMontageForContext(float Input, ETraversalHand Hand, bool bExiting, bool bEnter,
	bool bIsEntry, bool bIsExit) const
{
	if (bExiting)
	{
		if (bIsEntry)
			return Hand == ETraversalHand::Right ? AnimSet.ExitStartRight : AnimSet.ExitStartLeft;
		else
			return Hand == ETraversalHand::Right ? AnimSet.ExitEndRight : AnimSet.ExitEndLeft;
	}
	if (bEnter)
	{
		if (bIsEntry)
			return Hand == ETraversalHand::Right ? AnimSet.EnterStartRight : AnimSet.EnterStartLeft;
		else
			return Hand == ETraversalHand::Right ? AnimSet.EnterEndRight : AnimSet.EnterEndLeft;
	}

	if (Input > 0.f)
		return Hand == ETraversalHand::Right ? AnimSet.MoveForwardRight : AnimSet.MoveForwardLeft;
	else
		return Hand == ETraversalHand::Right ? AnimSet.MoveBackwardRight : AnimSet.MoveBackwardLeft;
}

void ATraversalActor::HandleTraversalNotify(ETraversalNotifyType EventType, class UTraversalComponent* Component)
{
}

void ATraversalActor::Interact_Implementation(AActor* Interactor)
{
	if (ITraversalInterface* TI = Cast<ITraversalInterface>(Interactor))
	{
		TI->TryStartTraversal(this);
	}
	
}

FText ATraversalActor::GetInteractionDisplayName_Implementation() const
{
	return IInteractionInterface::GetInteractionDisplayName_Implementation();
}
