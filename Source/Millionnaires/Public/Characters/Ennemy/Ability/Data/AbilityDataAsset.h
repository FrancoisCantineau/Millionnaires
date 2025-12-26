// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Ennemy/Ability/AbilityBase.h"
#include "Characters/Ennemy/Ability/Conditions/AbilityConditionBase.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"



/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	FName AbilityName;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	float MaxCooldown = 1.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Range")
	float RangeMin = 0.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Range")
	float RangeMax = 1000.0f;
    

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Ability Behavior")
	UAbilityBase* AbilityBehavior;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Data")
	float Damage = 0.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Data")
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Conditions")
	TArray<UAbilityConditionBase*> ActivationConditions;
	
	/** TAGS */
    
	/**
	 * Tags identifying this ability
	 * Ex: "Ability.Attack"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags", 
			  meta = (ToolTip = "Tags that identify this ability"))
	FGameplayTagContainer AbilityTags;
    
	/**
	 * Tags owner MUST have to trigger this ability
	 * Ex: "Status.Buff.Enraged"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Requirements",
			  meta = (ToolTip = "Owner must have ALL of these tags to use this ability"))
	FGameplayTagContainer RequiredTags;
    
	/**
	 * Tags disabling this ability
	 * Ex: "Status.Debuff.Stunned"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Requirements",
			  meta = (ToolTip = "Owner cannot have ANY of these tags to use this ability"))
	FGameplayTagContainer BlockedByTags;
    
	/**
	 * Tags added to the owner when the ability is done
	 * Ex: "Character.State.HasExploded"
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Effects",
			  meta = (ToolTip = "Tags granted to Owner after using this ability"))
	FGameplayTagContainer GrantedTags;
    
	/**
	 * Timer for given tag (-1 = infinite)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags|Effects",
			  meta = (EditCondition = "GrantedTags.Num() > 0", ClampMin = "-1.0",
					  ToolTip = "How long granted tags last (-1 = permanent)"))
	float GrantedTagsDuration = -1.0f;
};
