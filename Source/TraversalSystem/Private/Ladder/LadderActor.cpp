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

	// Auto-place EndPoint
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