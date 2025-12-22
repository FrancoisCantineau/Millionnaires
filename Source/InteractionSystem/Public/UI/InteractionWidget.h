#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

UCLASS()
class INTERACTIONSYSTEM_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Update the displayed interaction text */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionText(const FText& NewText);

	/** Show or hide the widget */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionVisible(bool bVisible);

protected:
	/** Text block that displays the interaction prompt */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractionText;
};