// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Ennemy/Ability/AbilityHandlerComponentBase.h"
#include "Characters/BaseCharacter.h"

// Sets default values for this component's properties
UAbilityHandlerComponentBase::UAbilityHandlerComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UAbilityHandlerComponentBase::BeginPlay()
{
	
	Super::BeginPlay();

	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	for (TSubclassOf<UAbilityBase> AbilityClass : AbilityClasses)
	{
		if (!AbilityClass) continue;

		UAbilityBase* Ability =
			NewObject<UAbilityBase>(this, AbilityClass);

		Ability->OwningCharacter = OwnerCharacter;
		Abilities.Add(Ability);
	}
}

bool UAbilityHandlerComponentBase::UseAbilityByIndex(int32 Index, AActor* Target)
{
	if (!Abilities.IsValidIndex(Index))
		return false;

	return Abilities[Index]->UseAbility(Target);
}


// Called every frame
void UAbilityHandlerComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

