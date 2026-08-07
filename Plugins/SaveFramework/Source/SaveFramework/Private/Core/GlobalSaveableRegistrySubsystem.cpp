#include "Core/GlobalSaveableRegistrySubsystem.h"
#include "Core/ISaveable.h"

void UGlobalSaveableRegistrySubsystem::RegisterGlobalSaveable(FName Key, UObject* Object)
{
	if (Key.IsNone() || !Object)
	{
		return;
	}

	if (!Object->Implements<USaveable>())
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveFramework: %s registered as a global saveable under key '%s' but doesn't implement ISaveable — it will be skipped by SaveGame()/LoadGame()."), *Object->GetName(), *Key.ToString());
	}

	if (Registered.Contains(Key))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveFramework: global saveable key '%s' registered twice — the previous entry is being replaced. Keys must be unique."), *Key.ToString());
	}

	Registered.Add(Key, Object);
}

void UGlobalSaveableRegistrySubsystem::UnregisterGlobalSaveable(FName Key)
{
	Registered.Remove(Key);
}
