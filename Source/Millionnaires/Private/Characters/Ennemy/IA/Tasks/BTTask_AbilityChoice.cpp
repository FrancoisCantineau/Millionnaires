// Fill out your copyright notice in the Description page of Project Settings.

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
		const FAbilityInfosStruct& Ability = BaseCharacter->AbilitiesSorted[i];
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(Ability.Ability);
		
		if (!Spec)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability %s not found in ASC"), 
				*Ability.Ability->GetName());
			continue;
		}
		
		if (!Spec->Ability)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability CDO is null for spec"));
			continue;
		}
		
		const bool bCanActivate = Spec->Ability->CanActivateAbility(
			Spec->Handle,
			ASC->AbilityActorInfo.Get()
		);
    
		UE_LOG(LogTemp, Log, TEXT("Ability %s -> %s"), 
			*Spec->Ability->GetName(),
			bCanActivate ? TEXT("READY") : TEXT("NOT READY")
		);
		
		if (bCanActivate && Distance > Ability.MinRange && Distance < Ability.MaxRange )
		{

			//ASC->AddLooseGameplayTag(SelectedAbilityTag);
			
			OwnerComp.GetBlackboardComponent()->SetValueAsInt(
		SelectedAbilityIndexKey.SelectedKeyName,
		i
			);
			
			UE_LOG(LogTemp, Log, TEXT("✅ Ability %s activated successfully"), 
			   *Spec->Ability->GetName());
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
