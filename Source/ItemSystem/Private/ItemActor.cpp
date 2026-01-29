#include "ItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "InventoryComponent.h"
#include "InventoryInterface.h"
#include "ItemData.h"

#pragma region Lifecycle

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;
   
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    
    RootComponent = MeshComponent;
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();
    SetupMesh();
}

#pragma endregion

#pragma region Public Interface

/**
 * Initialize item with data
 */
void AItemActor::InitializeItem(const FDataTableRowHandle& InItemHandle, int32 InStackAmount)
{
    ItemHandle = InItemHandle;
    StackAmount = FMath::Max(1, InStackAmount);
   
    if (!IsActorInitialized())
    {
       UE_LOG(LogTemp, Warning, TEXT("Item Actor initialization failed"));
       return;
    }

    SetupMesh();
}

/**
 * Get display name for interaction
 */
FText AItemActor::GetInteractionDisplayName_Implementation() const
{
    if (ItemHandle.IsNull())
    {
       return FText::FromString("Unknown Item");
    }

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("GetInteractionDisplayName"));
    if (!ItemData)
    {
       return FText::FromString("Unknown Item");
    }

    return ItemData->DisplayName;
}

/**
 * Handle pickup interaction
 */
void AItemActor::Interact_Implementation(AActor* InteractingActor)
{
   if (!InteractingActor)
   {
      UE_LOG(LogTemp, Warning, TEXT("Interacting Actor is NULL"));
      return;
   }

   if (ItemHandle.IsNull())
   {
      UE_LOG(LogTemp, Warning, TEXT("ItemActor has no valid ItemHandle"));
      return;
   }

   if (!InteractingActor->Implements<UInventoryInterface>())
   {
      UE_LOG(LogTemp, Warning, TEXT("Actor %s does not implement IInventoryInterface"), 
         *InteractingActor->GetName());
      return;
   }

   UE_LOG(LogTemp, Log, TEXT("Adding item to Character via IInventoryInterface"));

   const int32 RemainingAmount = IInventoryInterface::Execute_AddItem(
       InteractingActor,
       ItemHandle, 
       StackAmount
   );

   // Check pickup result
   if (RemainingAmount > 0)
   {
      if (RemainingAmount == StackAmount)
      {
         UE_LOG(LogTemp, Warning, TEXT("Inventory full, cannot pick up item"));
         return;
      }
       
      // Partially picked up
      StackAmount = RemainingAmount;
      UE_LOG(LogTemp, Log, TEXT("Partially picked up. Remaining: %d"), RemainingAmount);
      return;
   }
   
   // Fully picked up, destroy the item
   UE_LOG(LogTemp, Log, TEXT("Item fully picked up, destroying"));
   Destroy();
}

#pragma endregion

#pragma region Internal Helpers

/**
 * Setup visual mesh from item data
 */
void AItemActor::SetupMesh()
{
    if (ItemHandle.IsNull())
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemActor has no valid ItemHandle for SetupMesh"));
       return;
    }

    const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("SetupMesh"));
    if (!ItemData)
    {
       UE_LOG(LogTemp, Warning, TEXT("Failed to get ItemData for SetupMesh"));
       return;
    }
   
    if (ItemData->WorldMesh)
    {
       MeshComponent->SetStaticMesh(ItemData->WorldMesh);
    }
}

#pragma endregion