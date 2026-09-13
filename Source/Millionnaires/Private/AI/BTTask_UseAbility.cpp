// BTTask_UseAbility.cpp
#include "AI/BTTask_UseAbility.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_UseAbility::UBTTask_UseAbility()
{
	NodeName = TEXT("Use Ability");

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_UseAbility, TargetActorKey), AActor::StaticClass());
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_UseAbility, LocationKey));
}

EBTNodeResult::Type UBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!ControlledPawn || !BB || !EventTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayEventData Payload;
	Payload.Instigator = ControlledPawn;

	// An unassigned key selector already means "skip this part" — no separate toggle needed.
	if (!TargetActorKey.SelectedKeyName.IsNone())
	{
		Payload.Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	}

	if (!LocationKey.SelectedKeyName.IsNone())
	{
		const FVector Location = BB->GetValueAsVector(LocationKey.SelectedKeyName);

		FGameplayAbilityTargetData_LocationInfo* LocationData = new FGameplayAbilityTargetData_LocationInfo();
		LocationData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		LocationData->TargetLocation.LiteralTransform = FTransform(Location);

		Payload.TargetData.Add(LocationData);
	}

	// HandleGameplayEvent returns how many Abilities actually triggered — an honest signal,
	// unlike SendGameplayEventToActor which returns nothing and would always look like Success.
	const int32 ActivatedCount = ASC->HandleGameplayEvent(EventTag, &Payload);

	return ActivatedCount > 0 ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

#if WITH_EDITOR
FString UBTTask_UseAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Send %s"), *EventTag.ToString());
}
#endif