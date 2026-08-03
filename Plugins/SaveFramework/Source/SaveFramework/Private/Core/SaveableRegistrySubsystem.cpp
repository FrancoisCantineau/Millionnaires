#include "Core/SaveableRegistrySubsystem.h"
#include "Core/SaveGuidComponent.h"

void USaveableRegistrySubsystem::RegisterSaveable(USaveGuidComponent* Component)
{
	if (Component)
	{
		RegisteredComponents.AddUnique(Component);
	}
}

void USaveableRegistrySubsystem::UnregisterSaveable(USaveGuidComponent* Component)
{
	RegisteredComponents.RemoveSingleSwap(Component);
}
