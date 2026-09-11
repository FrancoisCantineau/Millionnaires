// BTTask_FleeUntilHidden.cpp
#include "AI/BTTask_FleeUntilHidden.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "FunctionLibrary/VisibilityLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_FleeUntilHidden::UBTTask_FleeUntilHidden()
{
	NodeName = TEXT("Flee Until Hidden");
	bNotifyTick = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeUntilHidden, TargetActorKey), AActor::StaticClass());
	EscapeLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeUntilHidden, EscapeLocationKey));
}

uint16 UBTTask_FleeUntilHidden::GetInstanceMemorySize() const
{
	return sizeof(FFleeMemory);
}

EBTNodeResult::Type UBTTask_FleeUntilHidden::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BB)
	{
		return EBTNodeResult::Failed;
	}

	const FVector EscapeLocation = BB->GetValueAsVector(EscapeLocationKey.SelectedKeyName);
	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(EscapeLocation);
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		return EBTNodeResult::Failed;
	}

	FFleeMemory* Memory = reinterpret_cast<FFleeMemory*>(NodeMemory);
	Memory->TimeSinceLastCheck = 0.f;
	Memory->TotalElapsed = 0.f;

	return EBTNodeResult::InProgress;
}

void UBTTask_FleeUntilHidden::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIController || !ControlledPawn || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FFleeMemory* Memory = reinterpret_cast<FFleeMemory*>(NodeMemory);
	Memory->TimeSinceLastCheck += DeltaSeconds;
	Memory->TotalElapsed += DeltaSeconds;

	if (Memory->TotalElapsed >= MaxDurationSeconds)
	{
		// Couldn't lose the target in time — give up rather than fleeing forever. The rest of
		// the tree decides what happens next (e.g. TargetVisible still true falls back to Chase).
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Throttle the visibility trace — no need to check every single frame.
	if (Memory->TimeSinceLastCheck < VisibilityCheckInterval)
	{
		return;
	}
	Memory->TimeSinceLastCheck = 0.f;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// V0: single watcher. See VisibilityLibrary — extending to "hidden from ALL players" later
	// only means building a bigger array here, this call itself never changes.
	const bool bCanSee = UVisibilityLibrary::CanAnyWatcherSeeActor({ TargetActor }, ControlledPawn);
	if (bCanSee)
	{
		return; // still visible — keep fleeing
	}

	// Hidden now — this Task's job is done. What happens NEXT (disappear for good, reappear
	// near the player, or just stay put quietly) is a separate decision that belongs one level
	// up in the BT, not hardcoded here — this Task only ever means "got hidden".
	AIController->StopMovement();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

#if WITH_EDITOR
FString UBTTask_FleeUntilHidden::GetStaticDescription() const
{
	return TEXT("Move to EscapeLocation, vanish once hidden from target");
}
#endif

EBTNodeResult::Type UBTTask_FleeUntilHidden::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->StopMovement();
	}

	return EBTNodeResult::Aborted;
}