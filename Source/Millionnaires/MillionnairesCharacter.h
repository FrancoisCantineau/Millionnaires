// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "InventoryComponent.h"
#include "DropComponent.h"
#include "InteractionComponent.h"
#include "InventoryWidget.h"

#include "MillionnairesCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMillionnairesCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	/** Inventory component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;
	
	/** Drop component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UDropComponent* DropComponent;

	/** Interaction component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UInteractionComponent* InteractionComponent;

	/** Input action for opening inventory */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InventoryAction;

	/** Input action for dropping item */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DropItemAction;
	
	/** Input action for interact */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 SelectedSlotIndex = INDEX_NONE;

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

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	AMillionnairesCharacter();

	/** Get inventory component */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetInventory() const { return InventoryComponent; }
	
	/** Get drop component */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UDropComponent* GetDropComponent() const { return DropComponent; }
	
	/** Select an inventory slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectInventorySlot(int32 SlotIndex);
	
	/** Get the currently selected inventory slot index */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	/** Cached inventory widget instance */
	UPROPERTY()
	UInventoryWidget* InventoryWidget;

	/** Is inventory currently open? */
	bool bIsInventoryOpen;
};