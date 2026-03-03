// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "BTTask_AbilityChoice" - Source
 * Notes: Decides which ability will be used. Mainly chose one then set the blackboard index to reference the ability. 
 */

#include "Characters/Ennemy/IA/Tasks/BTTask_AbilityChoice.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "VectorTypes.h"
#include "Characters/BaseCharacter.h"




EBTNodeResult::Type UBTTask_AbilityChoice::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
		return EBTNodeResult::Failed;

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(ControlledPawn);

	UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
	
	float Distance = FVector::Dist(
	ControlledPawn->GetActorLocation(),
	BaseCharacter->Target->GetActorLocation()
);
	
	for (int32 i = 0; i < BaseCharacter->AbilitiesSorted.Num(); ++i)
	{
		if (!BaseCharacter->AbilitiesSorted[i].CanBeCasted)
		{
			continue;
		}
		
		const FAbilityInfosStruct& Ability = BaseCharacter->AbilitiesSorted[i];
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(Ability.Ability);
		
		if (!Spec)
		{
			continue;
		}
		
		if (!Spec->Ability)
		{
			continue;
		}
		
		const bool bCanActivate = Spec->Ability->CanActivateAbility(
			Spec->Handle,
			ASC->AbilityActorInfo.Get()
		);
		
		if (bCanActivate /* && Distance > Ability.MinRange && Distance < Ability.MaxRange*/ )
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(
		SelectedAbilityIndexKey.SelectedKeyName,
		i
			);
			
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
