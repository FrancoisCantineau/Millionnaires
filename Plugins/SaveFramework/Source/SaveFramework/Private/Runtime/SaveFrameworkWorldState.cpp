#include "Runtime/SaveFrameworkWorldState.h"
#include "Core/SaveGuidComponent.h"
#include "Core/SaveableRegistrySubsystem.h"
#include "Core/ISaveable.h"
#include "GameFramework/Actor.h"

USaveGuidComponent* USaveFrameworkWorldState::FindLoadedComponent(const FGuid& TargetId) const
{
	const USaveableRegistrySubsystem* Registry = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveableRegistrySubsystem>() : nullptr;
	return Registry ? Registry->FindByGuid(TargetId) : nullptr;
}

void USaveFrameworkWorldState::SetState(const FGuid& TargetId, const FSaveableStatePatch& Patch)
{
	if (!TargetId.IsValid())
	{
		return;
	}

	USaveGuidComponent* Comp = FindLoadedComponent(TargetId);

	if (Patch.bHasTransform)
	{
		if (Comp)
		{
			Comp->ApplyTransform(Patch.Transform);
		}
		else
		{
			PendingStates.FindOrAdd(TargetId).Transform = Patch.Transform;
		}
	}

	if (Patch.bHasActive)
	{
		if (Comp)
		{
			Comp->ApplyActive(Patch.bActive);
		}
		else
		{
			PendingStates.FindOrAdd(TargetId).bActive = Patch.bActive;
		}
	}
}

void USaveFrameworkWorldState::ApplyCustomState(const FGuid& TargetId, const FInstancedStruct& CustomState)
{
	if (!TargetId.IsValid())
	{
		return;
	}

	if (const USaveGuidComponent* Comp = FindLoadedComponent(TargetId))
	{
		AActor* Actor = Comp->GetOwner();
		if (Actor && Actor->Implements<USaveable>())
		{
			ISaveable::Execute_RestoreState(Actor, CustomState);
		}
		else if (Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework: ApplyCustomState called on %s, which doesn't implement ISaveable."), *Actor->GetName());
		}
	}
	else
	{
		PendingCustomStates.Add(TargetId, CustomState);
	}
}

bool USaveFrameworkWorldState::ClaimState(const FGuid& TargetId, FSaveableGenericState& OutState)
{
	if (const FSaveableGenericState* Found = PendingStates.Find(TargetId))
	{
		OutState = *Found;
		return true;
	}
	return false;
}

bool USaveFrameworkWorldState::ClaimCustomState(const FGuid& TargetId, FInstancedStruct& OutState)
{
	if (const FInstancedStruct* Found = PendingCustomStates.Find(TargetId))
	{
		OutState = *Found;
		return true;
	}
	return false;
}
