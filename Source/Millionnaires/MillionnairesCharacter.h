// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Characters/DeathHandlerComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"
#include "GameFramework/Character.h"

//*GAS */
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/CharacterInterface.h"
#include "Data/CharacterDefinition.h"
#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"
#include "GameplayAbilitySystem/Attributes/StatusAttributeSet.h"

#include "MillionnairesCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMillionnairesCharacter : public ACharacter, public IAbilitySystemInterface, public ICharacterInterface
{
	GENERATED_BODY()

	

protected:

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer StateTags;
	

#pragma region Protected_GAS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS", meta = (AllowPrivateAccess = "true"))
	const  UBaseAttributeSet* BaseAttributesSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS", meta = (AllowPrivateAccess = "true"))
	const UStatusAttributeSet* StatusAttributesSet;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true", ToolTip = "Component that manages health, hunger and basic character data."))
	TObjectPtr<UCharacterStatsComponent> StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	UDeathHandlerComponent* DeathHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;
	
	UPROPERTY()
	bool bAttributesInitialized = false;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitialStatsEffect;

	/**Initialize the abilites */
	UFUNCTION(BlueprintCallable)
	void GiveAbilities();
	
	void InitAttributes();
    
	/** Called when HP is depleted and the character should die */
	virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

#pragma endregion 
	
public:

	AMillionnairesCharacter();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Returns the first person mesh */
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component */
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	//State tags//
	virtual void AddStateTag_Implementation(FGameplayTag StateTag);
	virtual void RemoveStateTag_Implementation(FGameplayTag StateTag);
	virtual bool HasStateTag_Implementation(FGameplayTag StateTag);

#pragma region Public_GAS
	
	//* Returns the ability system component for this actor */
	virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCharacterDefinition* DataCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FAbilityInfosStruct> AbilitiesSorted;

#pragma endregion
};