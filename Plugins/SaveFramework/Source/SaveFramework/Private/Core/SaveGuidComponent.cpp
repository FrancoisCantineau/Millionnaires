#include "Core/SaveGuidComponent.h"
#include "Core/SaveableRegistrySubsystem.h"
#include "Core/ISaveable.h"
#include "Runtime/SaveFrameworkWorldState.h"
#include "GameFramework/Actor.h"
#include "Engine/GameInstance.h"

USaveGuidComponent::USaveGuidComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(false);
}

void USaveGuidComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!Owner || !GameInstance)
	{
		return;
	}

	// Register unconditionally: generic state (position/active) is handled
	// right here, it doesn't require the owner to implement ISaveable.
	if (USaveableRegistrySubsystem* Registry = GameInstance->GetSubsystem<USaveableRegistrySubsystem>())
	{
		Registry->RegisterSaveable(this);
	}

	ClaimStateFromWorldState();
}

void USaveGuidComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (USaveableRegistrySubsystem* Registry = GameInstance->GetSubsystem<USaveableRegistrySubsystem>())
		{
			Registry->UnregisterSaveable(this);
		}
	}
	DepositStateIntoWorldState();

	Super::EndPlay(EndPlayReason);
}

void USaveGuidComponent::SimulateUnload()
{
	UE_LOG(LogTemp, Warning, TEXT("SaveFramework [DEBUG]: SimulateUnload on %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("?"));

	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (USaveableRegistrySubsystem* Registry = GameInstance->GetSubsystem<USaveableRegistrySubsystem>())
		{
			Registry->UnregisterSaveable(this);
		}
	}

	DepositStateIntoWorldState();
}

void USaveGuidComponent::SimulateReload()
{
	UE_LOG(LogTemp, Warning, TEXT("SaveFramework [DEBUG]: SimulateReload on %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("?"));

	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (USaveableRegistrySubsystem* Registry = GameInstance->GetSubsystem<USaveableRegistrySubsystem>())
		{
			Registry->RegisterSaveable(this);
		}
	}

	ClaimStateFromWorldState();
}

void USaveGuidComponent::DepositStateIntoWorldState()
{
	AActor* Owner = GetOwner();
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!Owner || !GameInstance)
	{
		return;
	}

	// Leave a note on the way out: whatever state this actor had while
	// it existed becomes the new "truth" for the next time it loads —
	// even if no SaveGame() happens in between. Without this, an actor
	// that was changed once (via SetState/ApplyCustomState) and then
	// unloads a second time without a fresh command would silently
	// forget the change and revert to its placed-in-editor state.
	if (USaveFrameworkWorldState* WorldState = GameInstance->GetSubsystem<USaveFrameworkWorldState>())
	{
		if (bAutoTrackGenericState)
		{
			FSaveableStatePatch Patch;
			Patch.bHasTransform = true;
			Patch.Transform = Owner->GetActorTransform();
			Patch.bHasActive = true;
			Patch.bActive = !Owner->IsHidden();
			WorldState->SetState(SaveId, Patch);
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework [DEBUG]: deposited transform for %s at %s"), *Owner->GetName(), *Patch.Transform.GetLocation().ToString());
		}

		// Custom state is always the actor's own responsibility to get
		// right (e.g. a platform captures "target node", not a raw
		// mid-flight transform) — no flag needed here, CaptureState()
		// itself is where that judgment call belongs.
		if (Owner->Implements<USaveable>())
		{
			WorldState->ApplyCustomState(SaveId, ISaveable::Execute_CaptureState(Owner));
		}
	}
}

void USaveGuidComponent::ClaimStateFromWorldState()
{
	AActor* Owner = GetOwner();
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!Owner || !GameInstance)
	{
		return;
	}

	// Claim ownership of whatever state was left for this actor while it
	// was unloaded — from this point on, this actor is the source of truth again.
	if (USaveFrameworkWorldState* WorldState = GameInstance->GetSubsystem<USaveFrameworkWorldState>())
	{
		FSaveableGenericState PendingState;
		if (WorldState->ClaimState(SaveId, PendingState))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework [DEBUG]: claimed transform for %s at %s"), *Owner->GetName(), *PendingState.Transform.GetLocation().ToString());
			ApplyTransform(PendingState.Transform);
			ApplyActive(PendingState.bActive);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveFramework [DEBUG]: nothing pending for %s, keeping current transform"), *Owner->GetName());
		}

		if (Owner->Implements<USaveable>())
		{
			FInstancedStruct PendingCustomState;
			if (WorldState->ClaimCustomState(SaveId, PendingCustomState))
			{
				ISaveable::Execute_RestoreState(Owner, PendingCustomState);
			}
		}
	}
}

void USaveGuidComponent::EnsureValidSaveId()
{
	if (!SaveId.IsValid())
	{
#if WITH_EDITOR
		Modify();
#endif
		SaveId = FGuid::NewGuid();
#if WITH_EDITOR
		if (AActor* Owner = GetOwner())
		{
			Owner->Modify();
			Owner->MarkPackageDirty();
		}
#endif
	}
}

void USaveGuidComponent::ApplyTransform(const FTransform& NewTransform)
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorTransform(NewTransform);
	}
}

void USaveGuidComponent::ApplyActive(bool bNewActive)
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorHiddenInGame(!bNewActive);
		Owner->SetActorEnableCollision(bNewActive);
	}
}

#if WITH_EDITOR

void USaveGuidComponent::OnRegister()
{
	Super::OnRegister();

	EnsurePersistentSaveId();
}

void USaveGuidComponent::EnsurePersistentSaveId()
{
	if (SaveId.IsValid())
	{
		return; // Déjà un GUID valide, on ne touche à rien !
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!World || World->WorldType != EWorldType::Editor)
	{
		return;
	}

	// Ne jamais générer pour les CDO / templates / archetypes
	if (Owner->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) ||
		HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		return;
	}

	// Marquer comme modifié pour forcer la sauvegarde
	Modify();
	Owner->Modify();

	// Générer le GUID
	SaveId = FGuid::NewGuid();

	// FORCER la sauvegarde immédiate dans le package
	if (UPackage* Package = Owner->GetOutermost())
	{
		Package->MarkPackageDirty();
	}
	Owner->MarkPackageDirty();
	MarkPackageDirty();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("SaveFramework: Generated and SAVED SaveId %s for %s"),
		*SaveId.ToString(),
		*Owner->GetPathName()
	);
}

#endif
