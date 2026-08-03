#include "Core/SaveGuidComponent.h"
#include "Core/SaveableRegistrySubsystem.h"
#include "Core/ISaveable.h"
#include "Runtime/SaveFrameworkWorldState.h"
#include "GameFramework/Actor.h"
#include "Engine/GameInstance.h"

USaveGuidComponent::USaveGuidComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	// Pick up any change that was made to this actor while it was unloaded.
	if (USaveFrameworkWorldState* WorldState = GameInstance->GetSubsystem<USaveFrameworkWorldState>())
	{
		FSaveableGenericState PendingState;
		if (WorldState->ConsumePendingState(SaveId, PendingState))
		{
			ApplyTransform(PendingState.Transform);
			ApplyActive(PendingState.bActive);
		}

		if (Owner->Implements<USaveable>())
		{
			FInstancedStruct PendingCustomState;
			if (WorldState->ConsumePendingCustomState(SaveId, PendingCustomState))
			{
				ISaveable::Execute_RestoreState(Owner, PendingCustomState);
			}
		}
	}
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

	Super::EndPlay(EndPlayReason);
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

	const UWorld* World = GetWorld();
	if (!SaveId.IsValid() && World && World->WorldType == EWorldType::Editor)
	{
		SaveId = FGuid::NewGuid();

		if (AActor* Owner = GetOwner())
		{
			Owner->Modify();
		}
	}
}
#endif
