// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryInterface.h"
#include "ItemData.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "QuestManagerComponent.h"
#include "UI/QuestTrackerWidget.h"

#include "MillionnairesCharacter.generated.h"

class UFlashlightEquipmentComponent;
class UInventoryComponent;
class URestrictedInventoryComponent;
class UDropComponent;
class UInteractionComponent;
class UMultiInventoryWidget;
class UInteractionWidget;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UConsumableComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMillionnairesCharacter : public ACharacter, public IInventoryInterface
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	void InitializeInventoryWidget();

#pragma region Inventory Operation Helper
	
	template<typename Func>
	bool TryInventoryOperation(const FItemData* ItemData, Func Operation) const
	{
		if (!ItemData) return false;

		if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
		{
			if (Operation(EquipmentInventoryComponent))
				return true;
		}
		else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
		{
			if (Operation(ConsumableInventoryComponent))
				return true;
		}

		if (GeneralInventoryComponent)
		{
			if (Operation(GeneralInventoryComponent))
				return true;
		}

		return false;
	}

#pragma endregion
	
#pragma region Input Actions

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MouseLookAction;

	/** Input action for opening inventory */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InventoryAction;

	/** Input action for dropping item */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DropItemAction;
	
	/** Input action for interact */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/** Input action for using health consumable (hotkey 1) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseHealthAction;

	/** Input action for using food consumable (hotkey 2) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseFoodAction;

	/** Input action for using battery consumable (hotkey 3) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseBatteryAction;

#pragma endregion

#pragma region Inventory

	/** General inventory - accepts all items EXCEPT specialized ones */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	URestrictedInventoryComponent* GeneralInventoryComponent;

	/** Equipment inventory - only accepts equipment items */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	URestrictedInventoryComponent* EquipmentInventoryComponent;

	/** Consumable inventory - only accepts consumable items */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	URestrictedInventoryComponent* ConsumableInventoryComponent;

	/** Drop component for dropping items */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UDropComponent* DropComponent;

#pragma endregion

#pragma region Interaction

	/** Interaction component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComponent;

	/** Interaction widget class */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	/** Interaction widget instance */
	UPROPERTY()
	UInteractionWidget* InteractionWidget;

#pragma endregion

#pragma region Consumable
	
	/** Consumable component for using items from inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UConsumableComponent* ConsumableComponent;
	
#pragma endregion
	
#pragma region Flashlight

	/** Flashlight equipment component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	UFlashlightEquipmentComponent* FlashlightComponent;

	/** Input action for toggling flashlight on/off */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleFlashlightAction;

	/** Input action for equipping/unequipping flashlight */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquipFlashlightAction;

#pragma endregion
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	UQuestManagerComponent* QuestManagerComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UQuestTrackerWidget> QuestTrackerWidgetClass;

	UPROPERTY()
	UQuestTrackerWidget* QuestTrackerWidget;
	
#pragma region UI

	/** Multi-inventory widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMultiInventoryWidget> MultiInventoryWidgetClass;

	/** Cached multi-inventory widget instance */
	UPROPERTY()
	UMultiInventoryWidget* MultiInventoryWidget;

	/** Is inventory currently open? */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bIsInventoryOpen = false;

#pragma endregion

#pragma region Selection State

	/** Currently selected slot index across all inventories */
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 SelectedSlotIndex = INDEX_NONE;

	/** Currently selected inventory component */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* SelectedInventoryComponent = nullptr;

#pragma endregion

#pragma region Input Handlers

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Called when inventory key is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnInventoryPressed();

	/** Called when drop key is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnDropItemPressed();

	/** Called when interact key is pressed */
	UFUNCTION()
	void OnInteractPressed();

	/** Called when a slot is selected in any inventory */
	UFUNCTION()
	void SelectInventorySlot(int32 SlotIndex, UInventoryComponent* InventoryComp);

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Called when health hotkey is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnUseHealthPressed();

	/** Called when food hotkey is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnUseFoodPressed();

	/** Called when battery hotkey is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnUseBatteryPressed();

	/** Called when consumable item is used from UI (right-click) */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UseConsumableFromSlot(int32 SlotIndex, UInventoryComponent* InventoryComp);
	
	/** Called when toggle flashlight key is pressed */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnToggleFlashlightPressed();

	/** Called when equip flashlight key is pressed (T) */
	UFUNCTION(BlueprintCallable, Category="Input")
	void OnEquipFlashlightPressed();
	
#pragma endregion

#pragma region Interface Implementations

	// IInventoryInterface implementations
	virtual int32 AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) override;
	virtual bool HasSpaceForItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;
	virtual bool HasSpaceForItemInAnyInventory_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount) const override;

#pragma endregion

public:

	AMillionnairesCharacter();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

#pragma region Public Getters

	/** Get general inventory component */
	FORCEINLINE URestrictedInventoryComponent* GetGeneralInventory() const { return GeneralInventoryComponent; }

	/** Get equipment inventory component */
	FORCEINLINE URestrictedInventoryComponent* GetEquipmentInventory() const { return EquipmentInventoryComponent; }

	/** Get consumable inventory component */
	FORCEINLINE URestrictedInventoryComponent* GetConsumableInventory() const { return ConsumableInventoryComponent; }
	
	/** Get drop component */
	FORCEINLINE UDropComponent* GetDropComponent() const { return DropComponent; }
	
	/** Get the currently selected inventory slot index */
	FORCEINLINE int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }

	/** Get the currently selected inventory component */
	FORCEINLINE UInventoryComponent* GetSelectedInventoryComponent() const { return SelectedInventoryComponent; }

	/** Returns the first person mesh */
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component */
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Get consumable component */
	FORCEINLINE UConsumableComponent* GetConsumableComponent() const { return ConsumableComponent; }
	
	/** Get flashlight component */
	FORCEINLINE UFlashlightEquipmentComponent* GetFlashlightComponent() const { return FlashlightComponent; }
	
	/** Get quest manager component */
	FORCEINLINE UQuestManagerComponent* GetQuestManager() const { return QuestManagerComponent; }

#pragma endregion
};