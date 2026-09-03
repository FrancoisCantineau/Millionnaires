// DialogueChoiceButtonBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueSystemTypes.h"
#include "DialogueChoiceButtonBase.generated.h"

class UTextBlock;
class UButton;
class UDialogueComponent;

/**
 * Base for a single choice button, spawned automatically by UDialogueWidgetBase — one instance
 * per available choice. Name a TextBlock "ChoiceLabel" and a Button "ChoiceButton" in your Widget
 * Blueprint's designer and the label/click wiring happens automatically; override SetupChoice in
 * Blueprint instead if you want custom behavior.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DIALOGUESYSTEM_API UDialogueChoiceButtonBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void SetupChoice(const FDialogueChoice& Choice, int32 ChoiceIndex, UDialogueComponent* OwningComponent);
	virtual void SetupChoice_Implementation(const FDialogueChoice& Choice, int32 ChoiceIndex, UDialogueComponent* OwningComponent);

protected:
	/** Auto-filled with Choice.ChoiceText if a TextBlock named exactly "ChoiceLabel" exists in your Widget Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ChoiceLabel;

	/** Auto-wired to call SelectChoice on the owning component if a Button named exactly "ChoiceButton" exists. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ChoiceButton;

private:
	UPROPERTY(Transient)
	TObjectPtr<UDialogueComponent> CachedComponent;

	int32 CachedIndex = INDEX_NONE;

	UFUNCTION()
	void HandleClicked();
};
