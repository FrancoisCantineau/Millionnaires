// BTTask_FirstReaction.cpp
#include "AI/BTTask_FirstReaction.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Components/Characters/LookAtComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FirstReaction::UBTTask_FirstReaction()
{
	NodeName = TEXT("First Reaction");
	bNotifyTick = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FirstReaction, TargetActorKey), AActor::StaticClass());
	ReactionPendingKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FirstReaction, ReactionPendingKey));
}

uint16 UBTTask_FirstReaction::GetInstanceMemorySize() const
{
	return sizeof(FFirstReactionMemory);
}

EBTNodeResult::Type UBTTask_FirstReaction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!ControlledPawn || !BB)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	if (!ControlledPawn->FindComponentByClass<ULookAtComponent>())
	{
		// No LookAtComponent on this Pawn — nothing to drive, fail cleanly rather than doing nothing silently.
		return EBTNodeResult::Failed;
	}

	FFirstReactionMemory* Memory = reinterpret_cast<FFirstReactionMemory*>(NodeMemory);
	Memory->PauseElapsed = 0.f;
	Memory->Step = EFirstReactionStep::WaitingToFaceTarget;

	return EBTNodeResult::InProgress;
}

void UBTTask_FirstReaction::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!ControlledPawn || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	ULookAtComponent* LookAtComp = ControlledPawn->FindComponentByClass<ULookAtComponent>();

	if (!TargetActor || !LookAtComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FFirstReactionMemory* Memory = reinterpret_cast<FFirstReactionMemory*>(NodeMemory);

	switch (Memory->Step)
	{
		case EFirstReactionStep::WaitingToFaceTarget:
		{
			LookAtComp->SetLookAtTarget(TargetActor->GetActorLocation());

			if (LookAtComp->GetLookAtAlpha() >= LookAtAlphaThreshold)
			{
				Memory->Step = EFirstReactionStep::Pausing;
				Memory->PauseElapsed = 0.f;
			}
			break;
		}
		case EFirstReactionStep::Pausing:
		{
			// Keep refreshing the target in case the player shifts slightly during the stare.
			LookAtComp->SetLookAtTarget(TargetActor->GetActorLocation());

			Memory->PauseElapsed += DeltaSeconds;
			if (Memory->PauseElapsed >= PauseDurationSeconds)
			{
				Memory->Step = EFirstReactionStep::Finished;
			}
			break;
		}
		case EFirstReactionStep::Finished:
		{
			LookAtComp->ClearLookAtTarget();
			BB->SetValueAsBool(ReactionPendingKey.SelectedKeyName, false);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			break;
		}
	}
}

#if WITH_EDITOR
FString UBTTask_FirstReaction::GetStaticDescription() const
{
	return FString::Printf(TEXT("Look at target, pause %.1fs"), PauseDurationSeconds);
}
#endif