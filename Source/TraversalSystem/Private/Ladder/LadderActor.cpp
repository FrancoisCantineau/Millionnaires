#include "Ladder/LadderActor.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Data/TraversalTypes.h"


ALadderActor::ALadderActor()
{
	TraversalType = ETraversalType::Ladder;
	TraversalAxis = FVector::UpVector;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);

	StepInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StepInstances"));
	StepInstances->SetupAttachment(RootComponent);
}

void ALadderActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!StepInstances) return;

	StepInstances->SetStaticMesh(StepMesh);
	StepInstances->ClearInstances();

	for (int32 i = 0; i < StepsCount; i++)
	{
		FTransform T;
		T.SetLocation(FVector(0.f, 0.f, i * StepSpacing));
		T.SetRotation(FQuat(FRotator(0.f, 90.f, 0.f)));
		StepInstances->AddInstance(T);
	}
	
	if (EndPoint)
	{
		EndPoint->SetRelativeLocation(
			FVector(0.f, 0.f, (StepsCount - 1) * StepSpacing)
		);
	}
}

void ALadderActor::HandleTraversalNotify(
	ETraversalNotifyType EventType,
	UTraversalComponent* Component
)
{
	if (!Component) return;

	switch (EventType)
	{
	case ETraversalNotifyType::StepLeft:
		Component->SetCurrentHand(ETraversalHand::Left);
		break;

	case ETraversalNotifyType::StepRight:
		Component->SetCurrentHand(ETraversalHand::Right);
		break;

	default:
		break;
	}
}

FText ALadderActor::GetInteractionDisplayName_Implementation() const
{
	return FText::FromString("Climb");
}

void ALadderActor::HandleTraversalInput(UTraversalComponent* Component, const FVector2D& Input)
{
	Super::HandleTraversalInput(Component, Input);

	if (Component->IsPlayingMontage()) return;
	if (FMath::Abs(Input.Y) < 0.1f) return;

	Input.Y > 0.f ? MoveForward(Component) : MoveBackward(Component);
}

void ALadderActor::MoveForward(UTraversalComponent* Component)
{
	if (!Component) return;

	bool bBlocked = Component->TraceInDirection(FVector::UpVector, StepSpacing);
	if (bBlocked) return;

	Component->UpdateExitPoint(FVector::UpVector, 120);

	bool bCanExit = Component->TraceFromPoint(
		GetExitPoint(),         
		GetExitPoint()->GetForwardVector(),
		100.f
	);

	if (!bCanExit)
	{
		Component->RequestMontage(1.f, true, false, false, true);
		Component->ExitTraversal();
	}
	else
		Component->RequestMontage(1.f, false, false, false, false);
}

// ============================================================
// BACKWARD = DOWN
// ============================================================

void ALadderActor::MoveBackward(UTraversalComponent* Component)
{
	if (!Component) return;

	bool bBlocked = Component->TraceInDirection(-FVector::UpVector, 150);
	if (bBlocked)
	{
		Component->RequestMontage(-1.f, true, false, true, false);
		Component->ExitTraversal();
		return;
	}

	Component->RequestMontage(-1.f, false, false, false, false);
}