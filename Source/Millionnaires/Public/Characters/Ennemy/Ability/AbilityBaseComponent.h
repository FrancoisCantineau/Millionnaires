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

#include "Components/ActorComponent.h"
#include "AbilityBaseComponent.generated.h"

class ABaseCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAbilityBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ResetCooldown();

	//** Properties */
	UPROPERTY()
	ABaseCharacter*  OwningCharacter;

	virtual bool CanUseAbility();

	float CurrentCooldown;

	float MaxCooldown = 4.f;

	bool bCanUseAbility = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool UseAbility();
	
};
