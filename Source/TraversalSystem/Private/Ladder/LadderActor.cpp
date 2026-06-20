#include "Ladder/LadderActor.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Data/TraversalTypes.h"


ALadderActor::ALadderActor()
{
	TraversalType = ETraversalType::Ladder;
	TraversalDirection = FVector::UpVector;

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

FTraversalEntryInfo ALadderActor::GetEntryInfo(ACharacter* Character, UTraversalComponent* Component) const
{
	FTraversalEntryInfo Info;
	Info.EntryPoint = const_cast<ALadderActor*>(this)->GetEntryPointForCharacter(Character);

	bool bFromBottom = (Info.EntryPoint == StartPoint);
	ETraversalHand Hand = Component->GetCurrentHand();

	Info.EnterTransition = bFromBottom
		? (Hand == ETraversalHand::Right ? AnimSet.EnterStartRight  : AnimSet.EnterStartLeft)
		: (Hand == ETraversalHand::Right ? AnimSet.EnterEndRight : AnimSet.EnterEndLeft);

	return Info;
}


void ALadderActor::MoveForward(UTraversalComponent* Component)
{
	if (!Component) return;

	const FVector Direction = GetTraversalDirection(Component);

	bool bBlocked = Component->TraceInDirectionForObstacle(Direction, ObstacleTraceDistance);
	if (bBlocked) return;

	Component->UpdateExitPoint(Direction, ExitPointDistance);

	FVector StartTracePosition = GetExitPoint()->GetComponentLocation();
	FVector EndTracePosition = StartTracePosition + GetExitPoint()->GetForwardVector() * ExitTraceDistance;
	
	if (!ExitPoint) return;

	bool bCanExit = Component->TraceFromPoint(
		StartTracePosition, EndTracePosition);

	if (!bCanExit)
	{
		Component->RequestExitTraversal(GetMontageForContext(1.f, Component->GetCurrentHand(), true, false, false, true));
	}
	else
	{
		Component->RequestMontage(1.f, false, false, false, false);
	}
}

// ============================================================
// BACKWARD = DOWN
// ============================================================

void ALadderActor::MoveBackward(UTraversalComponent* Component)
{
	if (!Component) return;

	bool bBlocked = Component->TraceInDirectionForObstacle(-FVector::UpVector, 150);
	if (bBlocked)
	{
		Component->RequestExitTraversal(GetMontageForContext(-1.f,Component->GetCurrentHand(), true, false, true, false));
		return;
	}

	Component->RequestMontage(-1.f, false, false, false, false);
}