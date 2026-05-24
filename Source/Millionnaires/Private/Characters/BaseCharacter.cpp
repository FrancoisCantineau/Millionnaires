/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "Francki"
 * Class: "BaseCharacter" - Source
 * Notes: Base character class shared by the player and AI. Contains generic logic and a stats component.
 */

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    //Add the ability system component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

    // Animation
    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
    ContextualAnimSceneActorComponent = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("ContextualAnimSceneActorComponent"));
    
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("BeginPlay called on: %s"), *GetName());
    UE_LOG(LogTemp, Warning, TEXT("DataCharacter: %s"), DataCharacter ? TEXT("OK") : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("InitAttributesEffect: %s"), (DataCharacter && DataCharacter->InitAttributesEffect) ? TEXT("OK") : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("GetMaxHealth: %f"), DataCharacter ? DataCharacter->GetMaxHealth() : -1.f);
    
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        BaseAttributesSet = AbilitySystemComponent->GetSet<UBaseAttributeSet>();
        StatusAttributesSet = AbilitySystemComponent->GetSet<UStatusAttributeSet>();

        
    }
    if (!bAttributesInitialized)
    {
        
        InitAttributes();
        GiveAbilities();
        bAttributesInitialized = true;
    }
    
    // Tag event registrations
    AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.Dead")).AddUObject(this, &ABaseCharacter::OnDeadTagChanged);
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
        GiveAbilities(); 
        InitAttributes();
        bAttributesInitialized = true;
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

void ABaseCharacter::InitAttributes()
{
    if (!AbilitySystemComponent || !DataCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("ASC ou DataCharacter NULL"));
        return;
    }

    InitialStatsEffect = DataCharacter->InitAttributesEffect;

    if (!InitialStatsEffect)
    {
        UE_LOG(LogTemp, Error, TEXT("InitialStatsEffect is NULL !"));
        return;
    }

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle Spec =
        AbilitySystemComponent->MakeOutgoingSpec(InitialStatsEffect, 1.f, Context);

    if (!Spec.IsValid())
        return;

    Spec.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag("Data.Base.Health.Current"),
        DataCharacter->GetMaxHealth());

    Spec.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag("Data.Base.Health.Max"),
        DataCharacter->GetMaxHealth());

    FActiveGameplayEffectHandle Handle =
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

    UE_LOG(LogTemp, Warning, TEXT("GE Handle valide: %s"), Handle.IsValid() ? TEXT("OUI") : TEXT("NON"));
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
    }

    
}


