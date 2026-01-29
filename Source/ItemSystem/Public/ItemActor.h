#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Engine/DataTable.h"

#include "ItemActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemPickedUp, AItemActor*, Item, AActor*, Picker);

/** 
 * Actor representing an item in the world that can be interacted with.
 * Holds item data and visual representation.
 */
UCLASS(Blueprintable, BlueprintType)
class ITEMSYSTEM_API AItemActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
    
	AItemActor();

	/** Visual mesh for the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* MeshComponent;

	/** Item data reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FDataTableRowHandle ItemHandle;

	/** Stack amount for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
	int32 StackAmount = 1;

protected:
    
	virtual void BeginPlay() override;

public:
    
	/** Initialize item with data */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeItem(const FDataTableRowHandle& InItemHandle, int32 InStackAmount);

	/** IInteractionInterface implementation */
	virtual FText GetInteractionDisplayName_Implementation() const override;
	virtual void Interact_Implementation(AActor* InteractingActor) override;

protected:
    
	/** Setup visual mesh from item data */
	void SetupMesh();
};