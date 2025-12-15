/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "BaseCharacter" - Source
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#include "Characters/BaseCharacter.h"

#include "Components/Characters/CharacterStatsComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("BPC_StatsComponent"));
}

void ABaseCharacter::ApplyDamage_Implementation(float Damage, AActor* DamageCauser)
{
    StatsComponent->ModifyHealth(Damage);
}
