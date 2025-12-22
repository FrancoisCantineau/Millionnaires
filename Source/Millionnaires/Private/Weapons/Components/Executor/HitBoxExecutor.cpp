// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Executor/HitBoxExecutor.h"

void UHitBoxExecutor::Initialize(AWeaponBase* Weapon)
{
	Super::Initialize(Weapon);

	if (!OwnerWeapon) return;
	
	Hitbox_Main = OwnerWeapon->FindComponentByClass<UBoxComponent>();
	if (!Hitbox_Main)
	{
		return;
	}

	Hitbox_Main->OnComponentBeginOverlap.AddDynamic(this, &UHitBoxExecutor::OnOverlap);
	Hitbox_Main->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hitbox_Main->SetGenerateOverlapEvents(true);
}

void UHitBoxExecutor::ExecuteAttack(float m_DamageMultiplier)
{
	Super::ExecuteAttack(m_DamageMultiplier);

	AlreadyHitActors.Empty();
	Hitbox_Main->SetGenerateOverlapEvents(true);
	Hitbox_Main->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Hitbox_Main->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



void UHitBoxExecutor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != OwnerWeapon->Owner && OtherActor != OwnerWeapon && !AlreadyHitActors.Contains(OtherActor))
	{
		AlreadyHitActors.Add(OtherActor);
		OnHit(SweepResult);
	}
}

void UHitBoxExecutor::EndAttackExecution()
{
	Super::EndAttackExecution();
	Hitbox_Main->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
