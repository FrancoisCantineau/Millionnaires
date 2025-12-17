// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Weapons/Components/Executor/AttackExecutorBase.h"
#include "HitBoxExecutor.generated.h"



/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UHitBoxExecutor : public UAttackExecutorBase
{
	GENERATED_BODY()

public:

	/** Properties*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hitbox")
	UBoxComponent* Hitbox_Main;


	//* Functions */

	void Initialize(AWeaponBase* Weapon) override;
	
	virtual void ExecuteAttack(float m_DamageMultiplier = 1) override;

protected:

	//* Functions */
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep,const FHitResult& SweepResult);

	virtual void EndAttackExecution() override;

	/** Properties*/
	
	UPROPERTY()
	TSet<AActor*> AlreadyHitActors;
	
};
