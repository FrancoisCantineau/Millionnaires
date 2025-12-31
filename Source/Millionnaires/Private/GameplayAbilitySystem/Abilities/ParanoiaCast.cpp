// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/ParanoiaCast.h"

void UParanoiaCast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/*if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}

	if (!ConcentrationMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Concentration Montage"));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			ConcentrationMontage
		);

	Task->OnCompleted.AddDynamic(this, &UGA_EnemyParanoia::OnConcentrationFinished);
	Task->OnInterrupted.AddDynamic(this, &UGA_EnemyParanoia::OnConcentrationCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_EnemyParanoia::OnConcentrationCancelled);

	Task->ReadyForActivation();*/
}
