/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "Francki"
 * Class: "BaseCharacter" - Source
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#include "Characters/BaseCharacter.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("BPC_StatsComponent"));
    DeathHandler = CreateDefaultSubobject<UDeathHandlerComponent>(TEXT("BPC_DeathHandlerComponent"));

    //Add the ability system component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
    
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(AbilitySystemComponent))
    {
        BaseAttributesSet = AbilitySystemComponent->GetSet<UBaseAttributeSet>();
        StatusAttributesSet = AbilitySystemComponent->GetSet<UStatusAttributeSet>();
    }

    // Tag event registrations
    AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.Dead")).AddUObject(this, &ABaseCharacter::OnDeadTagChanged);

    GiveAbilities();
}


UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABaseCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

/**
 * Deals with death tag update. Mainly call all the on death streamline
 * @param CallbackTag 
 * @param NewCount death state tag flag
 */
void ABaseCharacter::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        DeathHandler->ExecuteDeath();
    }
}

void ABaseCharacter::GiveAbilities()
{
    if (DataCharacter && AbilitySystemComponent)
    {

        AbilitiesSorted.Empty();
        
        for (const FAbilityInfosStruct& Entry : DataCharacter->Abilities)
        {
            if (!Entry.Ability || !Entry.ShouldActivate)
            {
                continue;
            }

            AbilitiesSorted.Add(Entry);

            FGameplayAbilitySpec Spec(
                Entry.Ability,
                1,          
                INDEX_NONE,
                this      
            );

            AbilitySystemComponent->GiveAbility(Spec);
        }
        AbilitiesSorted.Sort([](const FAbilityInfosStruct& A, const FAbilityInfosStruct& B)
    {
        return A.Priority < B.Priority;
    });

        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    
}


