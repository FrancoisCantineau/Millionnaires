// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Attack/BurstAttackModeComponent.h"


void UBurstAttackModeComponent::Attack()
{
	if (!CanAttack())
		return;

	if (ShotsFired == 0)
	{
		ShotsFired = 0;
		TimeOfLastAttack = GetWorld()->GetTimeSeconds();

		Super::Attack();

		ShotsFired++;
		
		if (ShotsPerBurst > 1)
		{
			GetWorld()->GetTimerManager().SetTimer(
				BurstTimerHandle,
				this,
				&UBurstAttackModeComponent::Attack,
				TimeBetweenBurstShots,
				true
			);
		}
	}
	else
	{
		Super::Attack();
		ShotsFired++;
	}
	if (ShotsFired >= ShotsPerBurst)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		ShotsFired = 0;
	}
}

void UBurstAttackModeComponent::StopAttacking()
{
	Super::StopAttacking();
}
