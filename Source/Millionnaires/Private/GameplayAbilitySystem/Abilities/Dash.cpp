// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Abilities/Dash.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/RootMotionSource.h"


void UDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	FVector DashDir = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	UAbilityTask_ApplyRootMotionConstantForce* Task =
	UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		DashDir,
		DashStrength,
		DashDuration,
		true,            
		nullptr,         
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector::ZeroVector,
		0.f,
		false          
	);
	Task->OnFinish.AddDynamic(this, &UDash::OnDashFinished);
	Task->ReadyForActivation();
}

void UDash::OnDashFinished()
{
	EndAbility(
	   CurrentSpecHandle,
	   CurrentActorInfo,
	   CurrentActivationInfo,
	   false, 
	   false  
   );
}
