#include "DropComponent.h"

#include "ItemActor.h"
#include "InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#pragma region Lifecycle

UDropComponent::UDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#pragma endregion

#pragma region Public API

/** 
 * Drops the item contained in the given inventory slot.
 */
bool UDropComponent::DropFromInventory(UInventoryComponent* Inventory, int32 SlotIndex)
{
	if (!Inventory || !Inventory->IsValidSlotIndex(SlotIndex))
	{
		return false;
	}

	const FItemSlot Slot = Inventory->GetSlot(SlotIndex);
	if (Slot.IsEmpty())
	{
		return false;
	}

	AItemActor* DroppedItem = SpawnItemActor(Slot.ItemHandle, Slot.StackAmount);
	if (!DroppedItem)
	{
		return false;
	}
	
	if (!Inventory->RemoveItem(SlotIndex))
	{
		DroppedItem->Destroy();
		return false;
	}

	return true;
}

/** 
 * Drops a specific item without using an inventory.
 */
bool UDropComponent::DropItem(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
	if (ItemHandle.IsNull() || Amount <= 0)
	{
		return false;
	}

	return SpawnItemActor(ItemHandle, Amount) != nullptr;
}

#pragma endregion

#pragma region Internal Helpers

/** 
 * Computes a safe spawn location in front of the owner.
 */
bool UDropComponent::ComputeDropTransform(FVector& OutLocation, FRotator& OutRotation) const
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();

	if (!World || !Owner)
	{
		return false;
	}

	// Base drop direction: always relative to the owning actor
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector ForwardVector = Owner->GetActorForwardVector();

	FVector DesiredLocation = OwnerLocation + ForwardVector * DropDistance;

	// Line trace to avoid spawning inside walls
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (World->LineTraceSingleByChannel(Hit, OwnerLocation, DesiredLocation, ECC_Visibility, Params))
	{
		DesiredLocation = Hit.Location + Hit.Normal * 30.0f;
	}

	OutLocation = DesiredLocation + FVector::UpVector * 50.0f;
	OutRotation = FRotator::ZeroRotator;

	return true;
}

/** 
 * Spawns an item actor and initializes it.
 */
AItemActor* UDropComponent::SpawnItemActor(const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
	if (!ItemActorClass)
	{
		return nullptr;
	}

	FVector SpawnLocation;
	FRotator SpawnRotation;

	if (!ComputeDropTransform(SpawnLocation, SpawnRotation))
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AItemActor* ItemActor = World->SpawnActor<AItemActor>(
		ItemActorClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	if (!ItemActor)
	{
		return nullptr;
	}

	ItemActor->InitializeItem(ItemHandle, Amount);

	return ItemActor;
}

#pragma endregion
