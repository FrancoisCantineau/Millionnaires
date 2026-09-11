// BTTask_SetMovementSpeed.cpp
#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "SpeedProfileComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	USpeedProfileComponent* SpeedComp = ControlledPawn->FindComponentByClass<USpeedProfileComponent>();
	if (!SpeedComp)
	{
		return EBTNodeResult::Failed;
	}

	return SpeedComp->SetSpeedProfile(SpeedProfile) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

#if WITH_EDITOR
FString UBTTask_SetMovementSpeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Profile = %s"), *SpeedProfile.ToString());
}
#endif
