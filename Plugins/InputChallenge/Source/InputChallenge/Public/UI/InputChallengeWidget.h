#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputChallengeWidget.generated.h"

class UInputGlyphWidget;
class UInputChallengeComponent;
class UInputAction;

UCLASS()
class INPUTCHALLENGE_API UInputChallengeWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void Init(UInputChallengeComponent* InComponent);

protected:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputGlyphWidget> GlyphWidget;

	UFUNCTION()
	void HandleProgress(float Value);

	UFUNCTION()
	void HandleExpectedActionChanged(UInputAction* Action);

	UFUNCTION()
	void HandleSuccess();

	UFUNCTION()
	void HandleFailure();
};