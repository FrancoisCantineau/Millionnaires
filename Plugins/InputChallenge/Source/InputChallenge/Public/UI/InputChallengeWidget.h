#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputChallengeWidget.generated.h"

class UInputGlyphWidget;
class UInputChallengeComponent;
class UInputAction;

class UTextBlock;

UCLASS()
class INPUTCHALLENGE_API UInputChallengeWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void Init(UInputChallengeComponent* InComponent);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

protected:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputGlyphWidget> GlyphWidget;

	/** Optional - shows the countdown while the challenge is running, if bound in the Designer. */
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> TimerText;

	UFUNCTION()
	void HandleProgress(float Value);

	UFUNCTION()
	void HandleExpectedActionChanged(UInputAction* Action);

	UFUNCTION()
	void HandleSuccess();

	UFUNCTION()
	void HandleFailure();

private:

	/** Kept so NativeDestruct can unbind from the exact same delegates Init bound to. */
	UPROPERTY()
	TObjectPtr<UInputChallengeComponent> BoundComponent;
};