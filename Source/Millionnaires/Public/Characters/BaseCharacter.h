/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "Francois"
 * Class: "BaseCharacter" - Header
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/DamageableInterface.h"
#include "Ennemy/Ability/AbilityBaseComponent.h"

#include "BaseCharacter.generated.h"

class UCharacterStatsComponent;

UCLASS(Abstract)
class MILLIONNAIRES_API ABaseCharacter : public ACharacter, public IDamageableInterface
{
    GENERATED_BODY()

public:

    ABaseCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "Character|Components")
    UCharacterStatsComponent* GetStatsComponent() const { return StatsComponent; }

    //* Damages taken interface */
    virtual void ApplyDamage_Implementation(float Damage,AActor* DamageCauser) override;

    //** Properties */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    TArray<UAbilityBaseComponent*> AbilityComponents;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components", meta = (AllowPrivateAccess = "true", ToolTip = "Component that manages health, hunger and basic character data."))
    TObjectPtr<UCharacterStatsComponent> StatsComponent;

    
};
