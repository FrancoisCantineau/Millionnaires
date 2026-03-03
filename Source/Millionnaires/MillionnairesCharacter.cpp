// Copyright Epic Games, Inc. All Rights Reserved.

#include "MillionnairesCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Characters/CharacterStatsComponent.h"

#pragma region Command

static FAutoConsoleCommand CCmdDamage(
	TEXT("test.damage"),
	TEXT("Deal 20 damage to player"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (!GEngine || !GEngine->GameViewport)
		{
			UE_LOG(LogTemp, Error, TEXT("No GEngine or GameViewport!"));
			return;
		}

		UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("No World found!"));
			return;
		}

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			UE_LOG(LogTemp, Error, TEXT("No PlayerController found!"));
			return;
		}

		APawn* Pawn = PC->GetPawn();
		if (!Pawn)
		{
			UE_LOG(LogTemp, Error, TEXT("No Pawn found!"));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Found Pawn: %s"), *Pawn->GetName());

		UCharacterStatsComponent* Stats = Pawn->FindComponentByClass<UCharacterStatsComponent>();
		if (!Stats)
		{
			UE_LOG(LogTemp, Error, TEXT("No CharacterStatsComponent found on pawn!"));
            
			// List all components
			TArray<UActorComponent*> Components;
			Pawn->GetComponents(Components);
			UE_LOG(LogTemp, Warning, TEXT("Pawn has %d components:"), Components.Num());
			for (UActorComponent* Comp : Components)
			{
				UE_LOG(LogTemp, Warning, TEXT("  - %s"), *Comp->GetName());
			}
			return;
		}

		float HealthBefore = Stats->GetCurrentHealth();
		Stats->ModifyHealth(-20.0f);
		float HealthAfter = Stats->GetCurrentHealth();
        
		UE_LOG(LogTemp, Warning, TEXT("✓ DAMAGE APPLIED: %.1f -> %.1f (%.1f damage)"), 
			HealthBefore, HealthAfter, HealthBefore - HealthAfter);
	})
);

#pragma endregion 

AMillionnairesCharacter::AMillionnairesCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;



	
	//Add the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	

}

void AMillionnairesCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.Dead")).AddUObject(this, &AMillionnairesCharacter::OnDeadTagChanged);
	
}

UAbilitySystemComponent* AMillionnairesCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMillionnairesCharacter::InitAttributes()
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
void AMillionnairesCharacter::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		DeathHandler->ExecuteDeath();
	}
}

void AMillionnairesCharacter::GiveAbilities()
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

void AMillionnairesCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
    
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AMillionnairesCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
