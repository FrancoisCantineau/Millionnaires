#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"

#include "InventoryInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors/components that can hold inventory
 */
class GAMEINTERFACES_API IInventoryInterface
{
	GENERATED_BODY()

public:

	/** Add an item to the inventory */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	int32 AddItem(const FDataTableRowHandle& ItemHandle, int32 Amount);

	/** Check if there's space for an item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool HasSpaceForItem(const FDataTableRowHandle& ItemHandle, int32 Amount) const;
	
};