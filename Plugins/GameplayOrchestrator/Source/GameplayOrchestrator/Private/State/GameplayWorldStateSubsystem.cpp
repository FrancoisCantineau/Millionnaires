#include "State/GameplayWorldStateSubsystem.h"
#include "Core/StatefulActorInterface.h"
#include "StructUtils/InstancedStruct.h"

void UGameplayWorldStateSubsystem::SetState(FGameplayTag Key, FGameplayTag Value, EWorldStatePersistence Persistence)
{
	if (!Key.IsValid())
	{
		return;
	}

	States.Add(Key, Value);
	Policies.Add(Key, Persistence);
	NotifyRegisteredActors(Key, Value);
}

FGameplayTag UGameplayWorldStateSubsystem::GetState(FGameplayTag Key) const
{
	if (const FGameplayTag* Found = States.Find(Key))
	{
		return *Found;
	}
	return FGameplayTag::EmptyTag;
}

bool UGameplayWorldStateSubsystem::HasState(FGameplayTag Key) const
{
	return States.Contains(Key);
}

void UGameplayWorldStateSubsystem::SetExtendedState(FGameplayTag Key, const FInstancedStruct& Value, EWorldStatePersistence Persistence)
{
	if (!Key.IsValid())
	{
		return;
	}
	ExtendedStates.Add(Key, Value);
	Policies.Add(Key, Persistence);
}

FInstancedStruct UGameplayWorldStateSubsystem::GetExtendedState(FGameplayTag Key) const
{
	if (const FInstancedStruct* Found = ExtendedStates.Find(Key))
	{
		return *Found;
	}
	return FInstancedStruct();
}

void UGameplayWorldStateSubsystem::RegisterActor(FGameplayTag IdentityTag, AActor* Actor)
{
	if (!IdentityTag.IsValid() || !IsValid(Actor))
	{
		return;
	}

	RegisteredActors.AddUnique(IdentityTag, Actor);

	if (const FGameplayTag* ExistingState = States.Find(IdentityTag))
	{
		if (Actor->GetClass()->ImplementsInterface(UStatefulActor::StaticClass()))
		{
			IStatefulActor::Execute_ApplyState(Actor, *ExistingState);
		}
	}
}

void UGameplayWorldStateSubsystem::UnregisterActor(FGameplayTag IdentityTag, AActor* Actor)
{
	if (!IdentityTag.IsValid() || Actor == nullptr)
	{
		return;
	}

	for (auto It = RegisteredActors.CreateKeyIterator(IdentityTag); It; ++It)
	{
		if (!It.Value().IsValid() || It.Value().Get() == Actor)
		{
			It.RemoveCurrent();
		}
	}
}

TArray<AActor*> UGameplayWorldStateSubsystem::GetRegisteredActors(FGameplayTag IdentityTag) const
{
	TArray<AActor*> Result;
	TArray<TWeakObjectPtr<AActor>> WeakArray;
	RegisteredActors.MultiFind(IdentityTag, WeakArray);

	for (const TWeakObjectPtr<AActor>& WeakActor : WeakArray)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			Result.Add(Actor);
		}
	}
	return Result;
}

void UGameplayWorldStateSubsystem::NotifyRegisteredActors(FGameplayTag Key, FGameplayTag NewValue)
{
	for (AActor* Actor : GetRegisteredActors(Key))
	{
		if (Actor->GetClass()->ImplementsInterface(UStatefulActor::StaticClass()))
		{
			IStatefulActor::Execute_ApplyState(Actor, NewValue);
		}
	}
}

FGameplayTag UGameplayWorldStateSubsystem::GetSaveSectionId() const
{
	return FGameplayTag::RequestGameplayTag(FName("Save.GameplayWorldStateSubsystem"), false);
}

FInstancedStruct UGameplayWorldStateSubsystem::CaptureState() const
{
	FWorldStateSnapshot Snapshot;

	// On n'inclut que les tags dont la policy n'est PAS RuntimeOnly.
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : States)
	{
		const EWorldStatePersistence* Policy = Policies.Find(Pair.Key);
		if (!Policy || *Policy != EWorldStatePersistence::RuntimeOnly)
		{
			Snapshot.States.Add(Pair.Key, Pair.Value);
			Snapshot.Policies.Add(Pair.Key, Policy ? *Policy : EWorldStatePersistence::SaveGame);
		}
	}

	for (const TPair<FGameplayTag, FInstancedStruct>& Pair : ExtendedStates)
	{
		const EWorldStatePersistence* Policy = Policies.Find(Pair.Key);
		if (!Policy || *Policy != EWorldStatePersistence::RuntimeOnly)
		{
			Snapshot.ExtendedStates.Add(Pair.Key, Pair.Value);
			Snapshot.Policies.Add(Pair.Key, Policy ? *Policy : EWorldStatePersistence::SaveGame);
		}
	}

	FInstancedStruct Wrapped;
	Wrapped.InitializeAs<FWorldStateSnapshot>(Snapshot);
	return Wrapped;
}

void UGameplayWorldStateSubsystem::RestoreState(const FInstancedStruct& InData)
{
	if (const FWorldStateSnapshot* Snapshot = InData.GetPtr<FWorldStateSnapshot>())
	{
		States = Snapshot->States;
		ExtendedStates = Snapshot->ExtendedStates;
		Policies = Snapshot->Policies;
		// Pas de notification aux Actors ici par design : au moment du Restore,
		// les niveaux n'ont généralement pas encore streamé leurs Actors. Ce sont
		// eux qui liront leur état à leur propre BeginPlay via RegisterActor.
	}
}

FString UGameplayWorldStateSubsystem::DebugDumpStates() const
{
	FString Out = TEXT("=== GameplayWorldStateSubsystem: simple states ===\n");
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : States)
	{
		const EWorldStatePersistence* Policy = Policies.Find(Pair.Key);
		const FString PolicyStr = Policy ? UEnum::GetDisplayValueAsText(*Policy).ToString() : TEXT("?");
		Out += FString::Printf(TEXT("  %s -> %s [%s]\n"), *Pair.Key.ToString(), *Pair.Value.ToString(), *PolicyStr);
	}

	Out += TEXT("=== GameplayWorldStateSubsystem: registered (loaded) actors ===\n");
	for (auto It = RegisteredActors.CreateConstIterator(); It; ++It)
	{
		const AActor* Actor = It.Value().Get();
		Out += FString::Printf(TEXT("  %s -> %s\n"), *It.Key().ToString(), Actor ? *Actor->GetName() : TEXT("<invalid>"));
	}

	return Out;
}
