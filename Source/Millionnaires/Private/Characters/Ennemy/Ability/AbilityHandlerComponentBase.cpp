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
	InitializeAbilities();
}


// Called every frame
void UAbilityHandlerComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FAbilityInstance& Instance : AbilityInstances)
	{
		if (Instance.CurrentCooldown > 0.0f)
		{
			Instance.CurrentCooldown -= DeltaTime;
            
			if (Instance.CurrentCooldown <= 0.0f)
			{
				Instance.CurrentCooldown = 0.0f;
				Instance.bCanUse = true;
			}
		}
	}
	// ...
}
bool UAbilityHandlerComponentBase::UseAbility(int32 AbilityIndex, AActor* Target)
{
	if (!CanUseAbility(AbilityIndex, Target))
		return false;
    
	FAbilityInstance& Instance = AbilityInstances[AbilityIndex];
	
	if (Instance.BehaviorInstance)
	{
		Instance.BehaviorInstance->ExecuteAbility(GetOwner(), Target, Instance.AbilityData);
	}
	
	Instance.bCanUse = false;
	Instance.CurrentCooldown = Instance.AbilityData->MaxCooldown;
    
	return true;
}

bool UAbilityHandlerComponentBase::TryActivateAbilityByTag(FGameplayTag Tag, AActor* Target, bool bActivateRandom)
{
	if (!Tag.IsValid() || !Target)
		return false;

	TArray<int32> ValidAbilityIndices = GetAbilitiesWithTag(Tag, true);

	if (ValidAbilityIndices.Num() == 0)
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: No usable ability found with tag '%s'"), 
			   *GetOwner()->GetName(), *Tag.ToString());
		return false;
	}
	
	int32 ChosenIndex;
	if (bActivateRandom)
	{
		ChosenIndex = ValidAbilityIndices[FMath::RandRange(0, ValidAbilityIndices.Num() - 1)];
	}
	else
	{
		ChosenIndex = ValidAbilityIndices[0];
	}
	
	return UseAbility(ChosenIndex, Target);
}

bool UAbilityHandlerComponentBase::UseBestAbility(AActor* Target)
{
	TArray<FAbilityInstance*> BestAbilities;
	int32 HighestPriority = MIN_int32;

	for (FAbilityInstance& Instance : AbilityInstances)
	{
		if (!Instance.AbilityData || !Instance.BehaviorInstance)
			continue;

		if (!Instance.bCanUse || Instance.CurrentCooldown > 0.f)
			continue;

		if (!IsInRange(Instance.AbilityData, Target))
			continue;

		if (!CheckAbilityConditions(Instance.AbilityData))
			continue;
		
		if (!CheckAbilityTags(Instance.AbilityData))
			continue;
		
		if (Instance.Priority > HighestPriority)
		{
			HighestPriority = Instance.Priority;
			BestAbilities.Empty();
			BestAbilities.Add(&Instance);
		}
		else if (Instance.Priority == HighestPriority)
		{
			BestAbilities.Add(&Instance);
		}
	}

	if (BestAbilities.Num() == 0)
		return false;

	
	FAbilityInstance* ChosenAbility =
		BestAbilities[FMath::RandRange(0, BestAbilities.Num() - 1)];

	
	ChosenAbility->BehaviorInstance->ExecuteAbility(
		GetOwner(),
		Target,
		ChosenAbility->AbilityData
	);

	GrantAbilityTags(ChosenAbility->AbilityData);
	ChosenAbility->bCanUse = false;
	ChosenAbility->CurrentCooldown = ChosenAbility->AbilityData->MaxCooldown;

	return true;
}

bool UAbilityHandlerComponentBase::CheckAbilityConditions(const UAbilityDataAsset* AbilityData) const
{
	if (!AbilityData)
		return false;
	
	if (AbilityData->ActivationConditions.Num() == 0)
		return true;

	AActor* Owner = GetOwner();
	if (!Owner)
		return false;
	
	for (UAbilityConditionBase* Condition : AbilityData->ActivationConditions)
	{
		if (Condition && !Condition->CheckCondition(Owner))
		{
			return false;
		}
	}
	
	return true;
}

bool UAbilityHandlerComponentBase::CheckAbilityTags(const UAbilityDataAsset* AbilityData) const
{
    
	if (AbilityData->BlockedByTags.Num() > 0)
	{
		if (OwnedTags.HasAny(AbilityData->BlockedByTags))
		{
			return false;
		}
	}
	
    
	if (AbilityData->RequiredTags.Num() > 0)
	{
		if (!OwnedTags.HasAll(AbilityData->RequiredTags))
		{
			return false;
		}
	}
    
	return true;
}

void UAbilityHandlerComponentBase::GrantAbilityTags(const UAbilityDataAsset* AbilityData)
{
	for (const FGameplayTag& Tag : AbilityData->GrantedTags)
	{
		AddGameplayTag(Tag, AbilityData->GrantedTagsDuration);
	}
}

void UAbilityHandlerComponentBase::AddGameplayTag(FGameplayTag Tag, float Duration)
{
	OwnedTags.AddTag(Tag);
	
}


bool UAbilityHandlerComponentBase::UseAbilityByName(FName AbilityName, AActor* Target)
{
	for (int32 i = 0; i < AbilityInstances.Num(); i++)
	{
		if (AbilityInstances[i].AbilityData->AbilityName == AbilityName)
		{
			return UseAbility(i, Target);
		}
	}
    
	return false;
}

bool UAbilityHandlerComponentBase::CanUseAbility(int32 AbilityIndex, AActor* Target) const
{
	if (!AbilityInstances.IsValidIndex(AbilityIndex))
		return false;
    
	const FAbilityInstance& Instance = AbilityInstances[AbilityIndex];
    
	if (!Instance.bCanUse || !Target)
		return false;
    
	
	if (!IsInRange(Instance.AbilityData, Target))
		return false;
	
	if (!CheckAbilityConditions(Instance.AbilityData))
		return false;
    
	return true;
}

TArray<int32> UAbilityHandlerComponentBase::GetAbilitiesWithTag(FGameplayTag Tag, bool bOnlyUsable) const
{
	TArray<int32> FoundIndices;

	if (!Tag.IsValid())
		return FoundIndices;

	for (int32 i = 0; i < AbilityInstances.Num(); i++)
	{
		const FAbilityInstance& Instance = AbilityInstances[i];
        
		if (!Instance.AbilityData)
			continue;
		
		if (AbilityHasTag(Instance.AbilityData, Tag))
		{
			if (bOnlyUsable)
			{
				if (Instance.bCanUse && Instance.CurrentCooldown <= 0.0f)
				{
					FoundIndices.Add(i);
				}
			}
			else
			{
				FoundIndices.Add(i);
			}
		}
	}

	return FoundIndices;
}

bool UAbilityHandlerComponentBase::AbilityHasTag(const UAbilityDataAsset* AbilityData, FGameplayTag Tag) const
{
	if (!AbilityData || !Tag.IsValid())
		return false;
    
	return AbilityData->AbilityTags.HasTag(Tag);
}

void UAbilityHandlerComponentBase::InitializeAbilities()
{
	AbilityInstances.Empty();
    
	for (const TPair<UAbilityDataAsset*, int32>& Pair : Abilities)
	{
		UAbilityDataAsset* AbilityData = Pair.Key;
		int32 Priority = Pair.Value;

		if (!AbilityData || !AbilityData->AbilityBehavior)
			continue;

		FAbilityInstance Instance;
		Instance.AbilityData = AbilityData;
		Instance.CurrentCooldown = 0.0f;
		Instance.bCanUse = true;
		Instance.Priority = Priority;
		
		Instance.BehaviorInstance = AbilityData->AbilityBehavior;

		AbilityInstances.Add(Instance);
	}
}

bool UAbilityHandlerComponentBase::IsInRange(const UAbilityDataAsset* AbilityData, AActor* Target) const
{
	if (!AbilityData || !Target || !GetOwner())
		return false;
    
	const float Distance = FVector::Dist(
		GetOwner()->GetActorLocation(),
		Target->GetActorLocation()
	);
    
	return Distance >= AbilityData->RangeMin && Distance <= AbilityData->RangeMax;
}
