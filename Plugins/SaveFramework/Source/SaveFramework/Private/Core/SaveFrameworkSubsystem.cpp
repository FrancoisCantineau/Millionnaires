#include "Core/SaveFrameworkSubsystem.h"
#include "Core/ISaveable.h"
#include "Core/SaveFrameworkSaveGame.h"
#include "Core/SaveableRegistrySubsystem.h"
#include "Core/SaveGuidComponent.h"
#include "Runtime/SaveFrameworkWorldState.h"
#include "Kismet/GameplayStatics.h"

void USaveFrameworkSubsystem::SaveGame(const FString& SlotName)
{
	USaveableRegistrySubsystem* Registry = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveableRegistrySubsystem>() : nullptr;
	USaveFrameworkWorldState* WorldState = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveFrameworkWorldState>() : nullptr;
	if (!Registry || !WorldState)
	{
		return;
	}

	// Start from whatever is already on disk (if anything), so an
	// unloaded actor doesn't lose its last known custom state just
	// because we can't recapture it right now.
	USaveFrameworkSaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		SaveGameObject = Cast<USaveFrameworkSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	}
	if (!SaveGameObject)
	{
		SaveGameObject = Cast<USaveFrameworkSaveGame>(UGameplayStatics::CreateSaveGameObject(USaveFrameworkSaveGame::StaticClass()));
	}
	if (!SaveGameObject)
	{
		return;
	}

	// 1) Every currently loaded saveable gets a fresh, complete record.
	for (const TWeakObjectPtr<USaveGuidComponent>& WeakComp : Registry->GetRegisteredComponents())
	{
		USaveGuidComponent* Comp = WeakComp.Get();
		AActor* Actor = Comp ? Comp->GetOwner() : nullptr;
		if (!Actor)
		{
			continue;
		}

		const FGuid Id = Comp->GetSaveId();
		if (!Id.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework: %s is registered but has no valid GUID, skipping."), *Actor->GetName());
			continue;
		}

		FSaveFrameworkRecord Record;
		Record.Transform = Actor->GetActorTransform();
		Record.bActive = !Actor->IsHidden();

		if (Actor->Implements<USaveable>())
		{
			Record.State = ISaveable::Execute_CaptureState(Actor);
		}

		SaveGameObject->Records.Add(Id, Record);
	}

	// 2) Anything still pending in the WorldState belongs to an actor that
	// is NOT currently loaded. Both channels (generic + custom) must still
	// reach disk, or they're lost the moment the game closes.
	for (const auto& Pair : WorldState->GetAllPendingStates())
	{
		FSaveFrameworkRecord& Record = SaveGameObject->Records.FindOrAdd(Pair.Key);
		Record.Transform = Pair.Value.Transform;
		Record.bActive = Pair.Value.bActive;
	}
	for (const auto& Pair : WorldState->GetAllPendingCustomStates())
	{
		FSaveFrameworkRecord& Record = SaveGameObject->Records.FindOrAdd(Pair.Key);
		Record.State = Pair.Value;
	}

	UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
}

void USaveFrameworkSubsystem::LoadGame(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		return;
	}

	USaveFrameworkSaveGame* SaveGameObject = Cast<USaveFrameworkSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (!SaveGameObject)
	{
		return;
	}

	USaveableRegistrySubsystem* Registry = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveableRegistrySubsystem>() : nullptr;
	USaveFrameworkWorldState* WorldState = GetGameInstance() ? GetGameInstance()->GetSubsystem<USaveFrameworkWorldState>() : nullptr;
	if (!Registry || !WorldState)
	{
		return;
	}

	for (const auto& Pair : SaveGameObject->Records)
	{
		const FGuid& Id = Pair.Key;
		const FSaveFrameworkRecord& Record = Pair.Value;

		USaveGuidComponent* LoadedComp = Registry->FindByGuid(Id);

		if (LoadedComp)
		{
			// Actor exists right now: apply immediately.
			LoadedComp->ApplyTransform(Record.Transform);
			LoadedComp->ApplyActive(Record.bActive);

			AActor* Actor = LoadedComp->GetOwner();
			if (Actor && Actor->Implements<USaveable>())
			{
				ISaveable::Execute_RestoreState(Actor, Record.State);
			}
		}
		else
		{
			// Actor isn't loaded: hand its state to the WorldState so both
			// channels get applied automatically whenever it eventually
			// loads — including later in this same session.
			FSaveableStatePatch Patch;
			Patch.bHasTransform = true;
			Patch.Transform = Record.Transform;
			Patch.bHasActive = true;
			Patch.bActive = Record.bActive;
			WorldState->SetState(Id, Patch);

			// Custom state may be empty (actor never implemented ISaveable) —
			// harmless to queue it regardless, SaveGuidComponent only
			// consumes it if the actor implements ISaveable when it loads.
			WorldState->ApplyCustomState(Id, Record.State);
		}
	}
}
