/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "Francki"
 * Class: "BaseCharacter" - Header
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/Characters/DeathHandlerComponent.h"
#include "GameFramework/Character.h"

//*GAS */
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Data/CharacterDefinition.h"
#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"
#include "GameplayAbilitySystem/Attributes/StatusAttributeSet.h"

//* ANIMATION */
#include "MotionWarpingComponent.h"
#include "ContextualAnimSceneActorComponent.h"


#include "Ennemy/Ability/AbilityHandlerComponentBase.h"


#include "BaseCharacter.generated.h"

class UContextComponent;
class UActionComponent;
class UCharacterStatsComponent;

UCLASS(Abstract)
class MILLIONNAIRES_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:

    ABaseCharacter(const FObjectInitializer& ObjectInitializer);

    //* Returns the ability system component for this actor */
    virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCharacterDefinition* DataCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAbilityInfosStruct> AbilitiesSorted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Target;


    
protected:

    //* GAS */
    
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

    void InitAttributes();

    //* GAS SYSTEM */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
    EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

    UPROPERTY()
    bool bAttributesInitialized = false;

    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> InitialStatsEffect;
    
    /** Called when HP is depleted and the character should die */
    virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

    /**Initialize the abilites */
    UFUNCTION(BlueprintCallable)
    void GiveAbilities();

    //* END GAS */

    virtual void BeginPlay() override;

    //* ANIMATION */

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UMotionWarpingComponent* MotionWarpingComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UContextualAnimSceneActorComponent* ContextualAnimSceneActorComponent;

    
};
