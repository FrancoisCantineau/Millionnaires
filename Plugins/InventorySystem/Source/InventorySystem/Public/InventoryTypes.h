// InventoryTypes.h
#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class UItemDefinition;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemDefinition> ItemDef;

	/** 0 when the slot is empty. Always 1 for Unique items — each unit is its own slot. Can go
	 *  up to MaxStackSize for Stackable items. */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;

	FORCEINLINE bool IsEmpty() const { return ItemDef == nullptr || Quantity <= 0; }

	void Clear()
	{
		ItemDef = nullptr;
		Quantity = 0;
	}
};

/** One line of "starting contents" you can author in the Details panel — see
 *  UInventoryComponent::InitialItems. */
USTRUCT(BlueprintType)
struct FInitialItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UItemDefinition> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

/** Self-contained save contract for UInventoryComponent — no dependency on any specific save
 *  plugin. Bridge this to your actual save system in project code. */
USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlot> Slots;
};
