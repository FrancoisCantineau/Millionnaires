// InventoryFunctionLibrary.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

class UInventoryComponent;
class UItemDefinition;

UCLASS()
class INVENTORYSYSTEM_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Moves up to Quantity units of Item from Source to Destination — e.g. player inventory to
	 *  a locker, or back. If Destination can't take everything (full), only transfers what fits
	 *  and the rest stays safely in Source — items are never lost mid-transfer. Returns how many
	 *  units were actually moved. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static int32 TransferItem(UInventoryComponent* Source, UInventoryComponent* Destination, UItemDefinition* Item, int32 Quantity);
};
