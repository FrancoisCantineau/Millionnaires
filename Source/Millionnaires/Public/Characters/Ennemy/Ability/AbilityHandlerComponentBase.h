// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Data/AbilityDataAsset.h"
#include "AbilityHandlerComponentBase.generated.h"

USTRUCT(BlueprintType)
struct FAbilityInstance
{
	GENERATED_BODY()
    
	UPROPERTY()
	UAbilityDataAsset* AbilityData = nullptr;
    
	UPROPERTY()
	class UAbilityBase* BehaviorInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Priority")
	int32 Priority = 1;
    
	float CurrentCooldown = 0.0f;
	bool bCanUse = true;

	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAbilityHandlerComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer OwnedTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability|Priority")
	TMap<UAbilityDataAsset*, int32> Abilities;
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool UseAbility(int32 AbilityIndex, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tags")
	bool TryActivateAbilityByTag(FGameplayTag Tag, AActor* Target, bool bActivateRandom = true);


	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool UseBestAbility(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool UseAbilityByName(FName AbilityName, AActor* Target);
    

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool CanUseAbility(int32 AbilityIndex, AActor* Target) const;


protected:

	UAbilityHandlerComponentBase();

	UPROPERTY(VisibleAnywhere)
	TArray<FAbilityInstance> AbilityInstances;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tags")
	TArray<int32> GetAbilitiesWithTag(FGameplayTag Tag, bool bOnlyUsable = false) const;

	bool AbilityHasTag(const UAbilityDataAsset* AbilityData, FGameplayTag Tag) const;

	
	void InitializeAbilities();
	
	bool IsInRange(const UAbilityDataAsset* AbilityData, AActor* Target) const;
	

	virtual void BeginPlay() override;

	bool CheckAbilityConditions(const UAbilityDataAsset* AbilityData) const;

	bool CheckAbilityTags(const UAbilityDataAsset* AbilityData) const;

	void GrantAbilityTags(const UAbilityDataAsset* AbilityData);

	void AddGameplayTag(FGameplayTag Tag, float Duration);

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
