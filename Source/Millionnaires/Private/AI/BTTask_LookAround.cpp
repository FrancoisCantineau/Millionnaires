// BTTask_LookAround.cpp
#include "AI/BTTask_LookAround.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Components/Characters/LookAtComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_LookAround::UBTTask_LookAround()
{
	NodeName = TEXT("Look Around");
	bNotifyTick = true;
}

uint16 UBTTask_LookAround::GetInstanceMemorySize() const
{
	return sizeof(FLookAroundMemory);
}

EBTNodeResult::Type UBTTask_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	if (!ControlledPawn->FindComponentByClass<ULookAtComponent>())
	{
		return EBTNodeResult::Failed;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FRotator PawnRotation = ControlledPawn->GetActorRotation();

	const FRotator LeftRotation = PawnRotation + FRotator(0.f, -LeftAmplitudeDegrees, 0.f);
	const FRotator RightRotation = LeftRotation + FRotator(0.f, RightSwingDegreesFromLeftExtreme, 0.f);

	FLookAroundMemory* Memory = reinterpret_cast<FLookAroundMemory*>(NodeMemory);
	Memory->LeftPoint = PawnLocation + LeftRotation.Vector() * LookDistance;
	Memory->RightPoint = PawnLocation + RightRotation.Vector() * LookDistance;
	Memory->PauseElapsed = 0.f;
	Memory->Step = ELookAroundStep::LookingLeft;

	return EBTNodeResult::InProgress;
}

void UBTTask_LookAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	ULookAtComponent* LookAtComp = ControlledPawn ? ControlledPawn->FindComponentByClass<ULookAtComponent>() : nullptr;

	if (!ControlledPawn || !LookAtComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FLookAroundMemory* Memory = reinterpret_cast<FLookAroundMemory*>(NodeMemory);

	switch (Memory->Step)
	{
		case ELookAroundStep::LookingLeft:
		{
			LookAtComp->SetLookAtTarget(Memory->LeftPoint);
			if (LookAtComp->GetLookAtAlpha() >= AlphaThreshold)
			{
				Memory->Step = ELookAroundStep::PausingAtLeft;
				Memory->PauseElapsed = 0.f;
			}
			break;
		}
		case ELookAroundStep::PausingAtLeft:
		{
			Memory->PauseElapsed += DeltaSeconds;
			if (Memory->PauseElapsed >= PauseDurationSeconds)
			{
				Memory->Step = ELookAroundStep::LookingRight;
			}
			break;
		}
		case ELookAroundStep::LookingRight:
		{
			// Alpha is likely already near 1 from the left look — the AnimGraph's own Look At
			// node handles smoothing the visual turn as the target point changes.
			LookAtComp->SetLookAtTarget(Memory->RightPoint);
			if (LookAtComp->GetLookAtAlpha() >= AlphaThreshold)
			{
				Memory->Step = ELookAroundStep::PausingAtRight;
				Memory->PauseElapsed = 0.f;
			}
			break;
		}
		case ELookAroundStep::PausingAtRight:
		{
			Memory->PauseElapsed += DeltaSeconds;
			if (Memory->PauseElapsed >= PauseDurationSeconds)
			{
				Memory->Step = ELookAroundStep::ReturningToCenter;
			}
			break;
		}
		case ELookAroundStep::ReturningToCenter:
		{
			LookAtComp->ClearLookAtTarget();
			if (LookAtComp->GetLookAtAlpha() <= (1.f - AlphaThreshold))
			{
				Memory->Step = ELookAroundStep::Finished;
			}
			break;
		}
		case ELookAroundStep::Finished:
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			break;
		}
	}
}

#if WITH_EDITOR
FString UBTTask_LookAround::GetStaticDescription() const
{
	return FString::Printf(TEXT("Look left %.0f°, swing right %.0f° (from left), pause %.1fs each side"),
		LeftAmplitudeDegrees, RightSwingDegreesFromLeftExtreme, PauseDurationSeconds);
}
#endif