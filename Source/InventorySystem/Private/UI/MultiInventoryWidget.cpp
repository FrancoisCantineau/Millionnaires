#include "UI/MultiInventoryWidget.h"
#include "UI/InventoryWidget.h"
#include "RestrictedInventoryComponent.h"
#include "Components/ActorComponent.h"

#pragma region Overrides

void UMultiInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!EquipmentInventoryComponent || !ConsumableInventoryComponent || !GeneralInventoryComponent)
    {
        AutoFindInventoryComponents();
    }
}

#pragma endregion

#pragma region Public Methods

/**
 * Initializes all inventory widgets with their respective inventory components.
 */
void UMultiInventoryWidget::InitializeAllInventories(
    URestrictedInventoryComponent* InEquipmentInventory,
    URestrictedInventoryComponent* InConsumableInventory,
    UInventoryComponent* InGeneralInventory)
{
    if (!InEquipmentInventory || !InConsumableInventory || !InGeneralInventory)
    {
        return;
    }

    EquipmentInventoryComponent = InEquipmentInventory;
    ConsumableInventoryComponent = InConsumableInventory;
    GeneralInventoryComponent = InGeneralInventory;

    SetupInventoryWidget(EquipmentInventory, EquipmentInventoryComponent);
    SetupInventoryWidget(ConsumableInventory, ConsumableInventoryComponent);
    SetupInventoryWidget(GeneralInventory, GeneralInventoryComponent);
}

/**
 * Automatically finds and assigns inventory components based on predefined tags.
 */
void UMultiInventoryWidget::AutoFindInventoryComponents()
{
    APawn* OwnerPawn = GetOwningPlayerPawn();
    if (!OwnerPawn)
    {
        return;
    }

    EquipmentInventoryComponent = FindInventoryByTag(FName("Equipment"));
    ConsumableInventoryComponent = FindInventoryByTag(FName("Consumable"));

    TArray<UInventoryComponent*> AllInventories;
    OwnerPawn->GetComponents<UInventoryComponent>(AllInventories);

    for (UInventoryComponent* Inventory : AllInventories)
    {
        if (!Inventory)
        {
            continue;
        }

        if (Inventory->GetClass() == UInventoryComponent::StaticClass())
        {
            GeneralInventoryComponent = Inventory;
            break;
        }

        if (URestrictedInventoryComponent* RestrictedInv = Cast<URestrictedInventoryComponent>(Inventory))
        {
            if (RestrictedInv->CategoryFilter.Categories.Num() == 0)
            {
                GeneralInventoryComponent = RestrictedInv;
                break;
            }
        }
    }

    if (EquipmentInventoryComponent && ConsumableInventoryComponent && GeneralInventoryComponent)
    {
        InitializeAllInventories(EquipmentInventoryComponent, ConsumableInventoryComponent, GeneralInventoryComponent);
    }
}

#pragma endregion

#pragma region Helpers

/**
 * Sets up a single inventory widget with the given inventory component.
 */
void UMultiInventoryWidget::SetupInventoryWidget(UInventoryWidget* Widget, UInventoryComponent* Component)
{
    if (Widget && Component)
    {
        Widget->InitializeInventory(Component);
    }
}

/**
 * Finds an inventory component by its tag.
 */
URestrictedInventoryComponent* UMultiInventoryWidget::FindInventoryByTag(const FName& Tag) const
{
    APawn* OwnerPawn = GetOwningPlayerPawn();
    if (!OwnerPawn)
    {
        return nullptr;
    }

    TArray<UActorComponent*> Components;
    OwnerPawn->GetComponents(URestrictedInventoryComponent::StaticClass(), Components);

    for (UActorComponent* Component : Components)
    {
        if (!Component)
        {
            continue;
        }

        if (Component->ComponentHasTag(Tag))
        {
            return Cast<URestrictedInventoryComponent>(Component);
        }
    }

    return nullptr;
}

#pragma endregion
