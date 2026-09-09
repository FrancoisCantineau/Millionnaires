// BTService_CheckHiddenFromTarget.cpp
#include "AI/BTService_CheckHiddenFromTarget.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "FunctionLibrary/VisibilityLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_CheckHiddenFromTarget::UBTService_CheckHiddenFromTarget()
{
	NodeName = TEXT("Check Hidden From Target");
	Interval = 0.2f;
	RandomDeviation = 0.f;
 
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckHiddenFromTarget, TargetActorKey), AActor::StaticClass());
	TargetCanSeeEnemyKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckHiddenFromTarget, TargetCanSeeEnemyKey));
}
 
void UBTService_CheckHiddenFromTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	GEngine->AddOnScreenDebugMessage(1, 4, FColor::Red, "Check Hidden From Target");
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
 
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
 
	if (!ControlledPawn || !BB)
	{
		return;
	}
 
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return;
	}
 
	// V0: single watcher (TargetActor) wrapped in a 1-element array. If multiplayer needs
	// "hidden from ALL players" later, build the array from every player's Pawn instead — the
	// function call below never needs to change.
	const bool bCanSee = UVisibilityLibrary::CanAnyWatcherSeeActor({ TargetActor }, ControlledPawn);
	BB->SetValueAsBool(TargetCanSeeEnemyKey.SelectedKeyName, bCanSee);
 
	// TEMP DEBUG — remove once the visibility check is confirmed working correctly.
	DrawDebugLine(
		AIController->GetWorld(),
		TargetActor->GetActorLocation(),
		ControlledPawn->GetActorLocation(),
		bCanSee ? FColor::Red : FColor::Green,
		false, 0.25f, 0, 2.f
	);
}