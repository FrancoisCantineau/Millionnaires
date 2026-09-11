// BTTask_SetDisappearState.cpp
#include "AI/BTTask_SetDisappearState.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_SetDisappearState::UBTTask_SetDisappearState()
{
	NodeName = TEXT("Set Disappear State");
}

EBTNodeResult::Type UBTTask_SetDisappearState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UDisappearComponent* DisappearComp = ControlledPawn->FindComponentByClass<UDisappearComponent>();
	if (!DisappearComp)
	{
		return EBTNodeResult::Failed;
	}

	DisappearComp->SetDisappearState(State);
	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UBTTask_SetDisappearState::GetStaticDescription() const
{
	return State == EDisappearState::Disappeared ? TEXT("Disappear") : TEXT("Appear");
}
#endif