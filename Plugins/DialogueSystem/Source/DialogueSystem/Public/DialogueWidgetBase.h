// DialogueWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueSystemTypes.h"
#include "DialogueChoiceButtonBase.h"
#include "DialogueWidgetBase.generated.h"

class UTextBlock;
class UImage;
class UPanelWidget;
class UDialogueComponent;

/**
 * Base class for a widget UDialogueComponent creates/updates/destroys automatically (opt-in —
 * only used if you assign a subclass to UDialogueComponent::WidgetClass).
 *
 * Name widgets in your Widget Blueprint's designer EXACTLY as below (matching type) and the C++
 * default implementation fills them in automatically — no event graph wiring needed for the
 * simple case:
 *   - TextBlock "LineText"            -> the subtitle
 *   - TextBlock "SpeakerNameText"     -> speaker's display name (colored per-speaker if set)
 *   - Image     "SpeakerPortraitImage" -> speaker's portrait (hidden if none)
 *   - A panel (VerticalBox, etc.) "ChoicesContainer" + ChoiceButtonClass assigned -> one
 *     auto-spawned UDialogueChoiceButtonBase per available choice
 * All of these are optional — leave any unnamed/unset and that piece is simply skipped.
 * Override the BlueprintNativeEvents in Blueprint instead for fully custom behavior.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class DIALOGUESYSTEM_API UDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Set automatically by UDialogueComponent when it creates this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogueComponent> OwningDialogueComponent;

	/** Widget class spawned once per available choice into ChoicesContainer. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|UI")
	TSubclassOf<UDialogueChoiceButtonBase> ChoiceButtonClass;

	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnLineStarted(const FDialogueLine& Line);
	virtual void OnLineStarted_Implementation(const FDialogueLine& Line);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnChoicesPresented(const TArray<FDialogueChoice>& Choices);
	virtual void OnChoicesPresented_Implementation(const TArray<FDialogueChoice>& Choices);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnDialogueEnded();
	virtual void OnDialogueEnded_Implementation();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LineText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeakerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> SpeakerPortraitImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ChoicesContainer;
};
