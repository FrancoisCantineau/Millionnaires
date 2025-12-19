// Copyright Epic Games, Inc. All Rights Reserved.

#include "MillionnairesCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Millionnaires.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "RestrictedInventoryComponent.h"
#include "DropComponent.h"
#include "MultiInventoryWidget.h"
#include "InteractionWidget.h"
#include "InventoryWidget.h"
#include "Blueprint/UserWidget.h"

AMillionnairesCharacter::AMillionnairesCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	GeneralInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("GeneralInventory"));
	GeneralInventoryComponent->NumSlots = 20;
	GeneralInventoryComponent->CategoryFilter.Categories = { EItemCategory::Equipment, EItemCategory::Consumable };
	GeneralInventoryComponent->CategoryFilter.bWhitelistMode = false;
	GeneralInventoryComponent->ComponentTags.Add(FName("General"));
	
	// Create equipment inventory (only equipment)
	EquipmentInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("EquipmentInventory"));
	EquipmentInventoryComponent->NumSlots = 10;
	EquipmentInventoryComponent->CategoryFilter.Categories = { EItemCategory::Equipment };
	EquipmentInventoryComponent->CategoryFilter.bWhitelistMode = true;
	EquipmentInventoryComponent->ComponentTags.Add(FName("Equipment"));

	// Create consumable inventory (only consumables)
	ConsumableInventoryComponent = CreateDefaultSubobject<URestrictedInventoryComponent>(TEXT("ConsumableInventory"));
	ConsumableInventoryComponent->NumSlots = 8;
	ConsumableInventoryComponent->CategoryFilter.Categories = { EItemCategory::Consumable };
	ConsumableInventoryComponent->CategoryFilter.bWhitelistMode = true;
	ConsumableInventoryComponent->ComponentTags.Add(FName("Consumable"));
	
	// Create drop component
	DropComponent = CreateDefaultSubobject<UDropComponent>(TEXT("DropComponent"));

	// Create interaction component
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->bShowDebugTrace = false;
}

void AMillionnairesCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
	
	if (InteractionWidget)
	{
		InteractionWidget->AddToViewport();
		InteractionWidget->SetInteractionVisible(false);
	}
}

void AMillionnairesCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InteractionWidget && InteractionComponent)
	{
		if (InteractionComponent->HasFocusedActor())
		{
			AActor* FocusedActor = InteractionComponent->GetFocusedActor();
			FText InteractionText = InteractionComponent->GetFocusedInteractionText();
			
			InteractionWidget->UpdateInteractionText(InteractionText);
			InteractionWidget->SetInteractionVisible(true);
		}
		else
		{
			InteractionWidget->SetInteractionVisible(false);
		}
	}
}

void AMillionnairesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMillionnairesCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMillionnairesCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMillionnairesCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMillionnairesCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMillionnairesCharacter::LookInput);
		
		// Inventory binding
		if (InventoryAction)
		{
			EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AMillionnairesCharacter::OnInventoryPressed);
		}

		// Drop item binding
		if (DropItemAction)
		{
			EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &AMillionnairesCharacter::OnDropItemPressed);
		}
		
		// Interact binding
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMillionnairesCharacter::OnInteractPressed);
		}
	}
	else
	{
		UE_LOG(LogMillionnaires, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

#pragma region Basic InputHandlers

void AMillionnairesCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AMillionnairesCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AMillionnairesCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMillionnairesCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AMillionnairesCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AMillionnairesCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

#pragma endregion

#pragma region Inventory / Drop / Interaction

/*
 * Inventory Handling
 * When the inventory key (Tab) is pressed, toggle the inventory UI
 */
void AMillionnairesCharacter::OnInventoryPressed()
{
	InitializeInventoryWidget();

	if (!MultiInventoryWidget) return;

	if (bIsInventoryOpen)
	{
		MultiInventoryWidget->RemoveFromParent();
		bIsInventoryOpen = false;

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
	else
	{
		MultiInventoryWidget->AddToViewport();
		bIsInventoryOpen = true;

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(MultiInventoryWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

/*
 * Select an inventory slot
 * When a slot is selected in the inventory UI, store the selected slot index
 */
void AMillionnairesCharacter::SelectInventorySlot(int32 SlotIndex, UInventoryComponent* InventoryComp)
{
	if (!InventoryComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing InventoryComponent"));
		return;
	}

	if (!InventoryComp->IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid slot selected: %d"), SlotIndex);
		return;
	}
	
	SelectedSlotIndex = SlotIndex;
	SelectedInventoryComponent = InventoryComp;
}

/*
 * Drop item from selected inventory slot
 * When the drop key (G) is pressed, drop the item in the selected slot
 */
void AMillionnairesCharacter::OnDropItemPressed()
{
	if (!SelectedInventoryComponent || !DropComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No slot selected or missing DropComponent"));
		return;
	}

	if (SelectedSlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("No inventory slot selected"));
		return;
	}

	if (!SelectedInventoryComponent->IsValidSlotIndex(SelectedSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid slot index: %d"), SelectedSlotIndex);
		return;
	}

	FItemSlot Slot = SelectedInventoryComponent->GetSlot(SelectedSlotIndex);
	if (Slot.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected slot %d is empty"), SelectedSlotIndex);
		return;
	}

	bool bDropSuccess = DropComponent->DropFromInventory(SelectedInventoryComponent, SelectedSlotIndex);

	if (bDropSuccess && SelectedInventoryComponent->GetSlot(SelectedSlotIndex).IsEmpty())
	{
		SelectedSlotIndex = INDEX_NONE;
		SelectedInventoryComponent = nullptr;
	}
}

/*
 * Interact with objects
 * When the interact key (E) is pressed, interact with the focused object
 */
void AMillionnairesCharacter::OnInteractPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->Interact();
	}
}

#pragma endregion

#pragma region Inventory Interface

/*
 * Inventory Interface Implementations
 */
bool AMillionnairesCharacter::HasSpaceForItemInAnyInventory_Implementation(
	const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
	const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("CheckSpace"));
	if (!ItemData)
	{
		return false;
	}

	if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
	{
		if (IInventoryInterface::Execute_HasSpaceForItem(EquipmentInventoryComponent, ItemHandle, Amount))
		{
			return true;
		}
	}
	else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
	{
		if (IInventoryInterface::Execute_HasSpaceForItem(ConsumableInventoryComponent, ItemHandle, Amount))
		{
			return true;
		}
	}

	if (GeneralInventoryComponent)
	{
		if (IInventoryInterface::Execute_HasSpaceForItem(GeneralInventoryComponent, ItemHandle, Amount))
		{
			return true;
		}
	}

	return false;
}

/*
 * Add item to appropriate inventory
 */
int32 AMillionnairesCharacter::AddItem_Implementation(const FDataTableRowHandle& ItemHandle, int32 Amount)
{
	if (ItemHandle.IsNull() || Amount <= 0)
	{
		return Amount;
	}

	const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("AddItem"));
	if (!ItemData)
	{
		return Amount;
	}

	int32 Remaining = Amount;

	auto AddToInventory = [&](UInventoryComponent* InvComp)
	{
		Remaining = IInventoryInterface::Execute_AddItem(InvComp, ItemHandle, Remaining);
	};

	if (ItemData->Category == EItemCategory::Equipment && EquipmentInventoryComponent)
	{
		AddToInventory(EquipmentInventoryComponent);
	}
	else if (ItemData->Category == EItemCategory::Consumable && ConsumableInventoryComponent)
	{
		AddToInventory(ConsumableInventoryComponent);
	}

	if (Remaining > 0 && GeneralInventoryComponent)
	{
		AddToInventory(GeneralInventoryComponent);
	}

	return Remaining;
}

/*
 * Check if there is space for item in appropriate inventory
 */
bool AMillionnairesCharacter::HasSpaceForItem_Implementation(
	const FDataTableRowHandle& ItemHandle, int32 Amount) const
{
	if (ItemHandle.IsNull() || Amount <= 0)
	{
		return false;
	}

	const FItemData* ItemData = ItemHandle.GetRow<FItemData>(TEXT("HasSpaceForItem"));
	if (!ItemData)
	{
		return false;
	}

	return this->TryInventoryOperation(ItemData, [&](UInventoryComponent* InvComp)
	{
		return IInventoryInterface::Execute_HasSpaceForItem(InvComp, ItemHandle, Amount);
	});
}

/*
 * Initialize the multi-inventory widget and bind slot selection events
 */
void AMillionnairesCharacter::InitializeInventoryWidget()
{
	if (!MultiInventoryWidgetClass || MultiInventoryWidget)
	{
		return;
	}

	MultiInventoryWidget = CreateWidget<UMultiInventoryWidget>(GetWorld(), MultiInventoryWidgetClass);
	if (!MultiInventoryWidget)
	{
		return;
	}

	MultiInventoryWidget->InitializeAllInventories(
		EquipmentInventoryComponent,
		ConsumableInventoryComponent,
		GeneralInventoryComponent
	);

	auto BindSlot = [&](UInventoryWidget* Widget)
	{
		if (Widget)
		{
			Widget->OnSlotSelected.AddDynamic(this, &AMillionnairesCharacter::SelectInventorySlot);
		}
	};

	BindSlot(MultiInventoryWidget->EquipmentInventory);
	BindSlot(MultiInventoryWidget->ConsumableInventory);
	BindSlot(MultiInventoryWidget->GeneralInventory);
}

#pragma endregion