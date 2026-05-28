#include "UI/InputChallengeWidget.h"
#include "EnhancedPlayerInput.h"

#include "InputGlyphWidget.h"
#include "Components/Image.h"

#include "InputChallengeComponent.h"

void UInputChallengeWidget::Init(UInputChallengeComponent* InComponent)
{
	if (!InComponent) return;

	InComponent->OnProgress.AddDynamic(this, &UInputChallengeWidget::HandleProgress);

	InComponent->OnExpectedActionChanged.AddDynamic(
		this,
		&UInputChallengeWidget::HandleExpectedActionChanged
	);

	InComponent->OnSuccess.AddDynamic(this, &UInputChallengeWidget::HandleSuccess);

	InComponent->OnFailure.AddDynamic(this, &UInputChallengeWidget::HandleFailure);

	HandleExpectedActionChanged(InComponent->GetCurrentExpectedAction());
}

void UInputChallengeWidget::HandleProgress(float Value)
{
	GlyphWidget->SetProgress(Value);
}

void UInputChallengeWidget::HandleExpectedActionChanged(UInputAction* Action)
{
	GlyphWidget->SetInputAction(Action);
}

void UInputChallengeWidget::HandleSuccess()
{
	//
}

void UInputChallengeWidget::HandleFailure()
{
	//
}

