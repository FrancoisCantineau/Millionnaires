#include "UI/InputChallengeWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "InputChallengeComponent.h"
#include "InputAction.h"

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
	if (ProgressBar)
	{
		ProgressBar->SetPercent(Value);
	}
}

void UInputChallengeWidget::HandleExpectedActionChanged(UInputAction* Action)
{
	if (!ActionText) return;

	if (!Action)
	{
		ActionText->SetText(FText::GetEmpty());
		return;
	}

	ActionText->SetText(
		FText::FromString(Action->GetName())
	);
}

void UInputChallengeWidget::HandleSuccess()
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(TEXT("SUCCESS")));
	}
}

void UInputChallengeWidget::HandleFailure()
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(TEXT("FAILED")));
	}
}