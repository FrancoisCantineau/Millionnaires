#include "TraversalActor.h"
#include "Components/ArrowComponent.h"
#include "TraversalInterface.h"


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

UAnimMontage* ATraversalActor::GetMontageForContext(
	float Input,
	ETraversalHand Hand,
	bool bExiting,
	bool bEnter, bool bIsEntry, bool bIsExit
) const
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

void ATraversalActor::HandleTraversalNotify(
	ETraversalNotifyType EventType,
	UTraversalComponent* Component
)
{
	// Base — rien, les subclasses overrident
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
	return FText::FromString("Traverse");
}