// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for abilities, used by the ennemies.
 */

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "AbilityBase.generated.h"

class ABaseCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAbilityBase : public UObject
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float RangeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float RangeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float MaxCooldown = 4.f;
	
	UPROPERTY()
	ABaseCharacter*  OwningCharacter;

protected:

	void ResetCooldown();

	float CurrentCooldown;

	bool bCanUseAbility = true;

	FTimerHandle CooldownTimerHandle;

	virtual void ExecuteAbility(AActor* Target);

public:	

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool UseAbility(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool CanUseAbility(AActor* Target);
};
