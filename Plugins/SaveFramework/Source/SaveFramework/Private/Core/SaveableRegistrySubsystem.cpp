#include "Core/SaveableRegistrySubsystem.h"
#include "Core/SaveGuidComponent.h"

void USaveableRegistrySubsystem::RegisterSaveable(USaveGuidComponent* Component)
{
	if (!Component)
	{
		return;
	}

	RegisteredComponents.AddUnique(Component);

	const FGuid Id = Component->GetSaveId();
	if (Id.IsValid())
	{
		ComponentsByGuid.Add(Id, Component);
	}
}

void USaveableRegistrySubsystem::UnregisterSaveable(USaveGuidComponent* Component)
{
	if (!Component)
	{
		return;
	}

	RegisteredComponents.RemoveSingleSwap(Component);

	const FGuid Id = Component->GetSaveId();
	if (Id.IsValid())
	{
		// Only remove if it's still this exact component at that key — avoids
		// accidentally evicting a different, newly-registered component that
		// happens to share the same GUID during a brief overlap.
		if (const TWeakObjectPtr<USaveGuidComponent>* Found = ComponentsByGuid.Find(Id))
		{
			if (Found->Get() == Component)
			{
				ComponentsByGuid.Remove(Id);
			}
		}
	}
}

USaveGuidComponent* USaveableRegistrySubsystem::FindByGuid(const FGuid& TargetId) const
{
	if (const TWeakObjectPtr<USaveGuidComponent>* Found = ComponentsByGuid.Find(TargetId))
	{
		return Found->Get();
	}
	return nullptr;
}
