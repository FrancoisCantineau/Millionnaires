#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"

#include "DropComponent.generated.h"

class AItemActor;
class UInventoryComponent;

/**
 * Component responsible for spawning dropped item actors in the world.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UDropComponent();

#pragma region Settings

	/** Actor class spawned when an item is dropped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Drop Settings")
	TSubclassOf<AItemActor> ItemActorClass;

	/** Forward distance from the owner where items are spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Settings")
	float DropDistance = 150.0f;

#pragma endregion

#pragma region Public API

	/** Drops the item contained in the given inventory slot. */
	UFUNCTION(BlueprintCallable, Category = "Drop")
	bool DropFromInventory(UInventoryComponent* Inventory, int32 SlotIndex);

	/** Drops a specific item without using an inventory. */
	UFUNCTION(BlueprintCallable, Category = "Drop")
	bool DropItem(const FDataTableRowHandle& ItemHandle, int32 Amount = 1);

#pragma endregion

private:

#pragma region Internal Helpers

	/** Computes a safe spawn location in front of the owner. */
	bool ComputeDropTransform(FVector& OutLocation, FRotator& OutRotation) const;

	/** Spawns an item actor and initializes it. */
	AItemActor* SpawnItemActor(const FDataTableRowHandle& ItemHandle, int32 Amount) const;

#pragma endregion
};