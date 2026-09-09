// BTTask_ChooseNeutralAction.cpp
#include "AI/BTTask_ChooseNeutralAction.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChooseNeutralAction::UBTTask_ChooseNeutralAction()
{
	NodeName = TEXT("Choose Neutral Action");

	NeutralActionKey.AddIntFilter(
		this,
		GET_MEMBER_NAME_CHECKED(UBTTask_ChooseNeutralAction, NeutralActionKey)
	);
}

EBTNodeResult::Type UBTTask_ChooseNeutralAction::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	const ENeutralAction Action =
		static_cast<ENeutralAction>(FMath::RandRange(0, static_cast<int32>(ENeutralAction::MAX) - 1));

	BlackboardComp->SetValueAsInt(
		NeutralActionKey.SelectedKeyName,
		static_cast<int32>(Action)
	);

	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FString UBTTask_ChooseNeutralAction::GetStaticDescription() const
{
	return TEXT("Randomly picks Wander, Idle, or Observe");
}
#endif