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
#include "GameplayAbilitySystem/Attributes/BaseAttributeSet.h"


#include "Interfaces/DamageableInterface.h"
#include "Ennemy/Ability/AbilityHandlerComponentBase.h"


#include "BaseCharacter.generated.h"

class UCharacterStatsComponent;

UCLASS(Abstract)
class MILLIONNAIRES_API ABaseCharacter : public ACharacter, public IDamageableInterface, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:

    ABaseCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "Character|Components")
    UCharacterStatsComponent* GetStatsComponent() const { return StatsComponent; }

    //* Damages taken interface */
    virtual void ApplyDamage_Implementation(float Damage,AActor* DamageCauser) override;

    //* Returns the ability system component for this actor */
    virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;


    
protected:

    //* GAS */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS", meta = (AllowPrivateAccess = "true"))
    const class UBaseAttributeSet* BaseAttributesSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS", meta = (AllowPrivateAccess = "true"))
    const class UWeaponAttributeSet* WeaponAttributesSet;
    

    //* END GAS */
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true", ToolTip = "Component that manages health, hunger and basic character data."))
    TObjectPtr<UCharacterStatsComponent> StatsComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
    UDeathHandlerComponent* DeathHandler;
    
    /** Name of the collision profile to use during ragdoll death */
    UPROPERTY(EditAnywhere, Category="Damage")
    FName RagdollCollisionProfile = FName("Ragdoll");

    //* GAS SYSTEM */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
    EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

    /** Called when HP is depleted and the character should die */
    UFUNCTION()
    void Die();

    virtual void BeginPlay() override;

    
};
