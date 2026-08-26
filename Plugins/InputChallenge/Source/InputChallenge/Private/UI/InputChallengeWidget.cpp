#include "UI/InputChallengeWidget.h"
#include "EnhancedPlayerInput.h"

#include "InputGlyphWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "InputChallengeComponent.h"

void UInputChallengeWidget::Init(UInputChallengeComponent* InComponent)
{
	if (!InComponent) return;

	BoundComponent = InComponent;

	InComponent->OnProgress.AddDynamic(this, &UInputChallengeWidget::HandleProgress);

	InComponent->OnExpectedActionChanged.AddDynamic(
		this,
		&UInputChallengeWidget::HandleExpectedActionChanged
	);

	InComponent->OnSuccess.AddDynamic(this, &UInputChallengeWidget::HandleSuccess);

	InComponent->OnFailure.AddDynamic(this, &UInputChallengeWidget::HandleFailure);

	HandleExpectedActionChanged(InComponent->GetCurrentExpectedAction());
}

void UInputChallengeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TimerText || !BoundComponent || !BoundComponent->IsActived())
	{
		return;
	}

	float Remaining = BoundComponent->GetRemainingTime();
	TimerText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FMath::Max(0.0f, Remaining))));
}

void UInputChallengeWidget::NativeDestruct()
{
	if (BoundComponent)
	{
		BoundComponent->OnProgress.RemoveDynamic(this, &UInputChallengeWidget::HandleProgress);
		BoundComponent->OnExpectedActionChanged.RemoveDynamic(this, &UInputChallengeWidget::HandleExpectedActionChanged);
		BoundComponent->OnSuccess.RemoveDynamic(this, &UInputChallengeWidget::HandleSuccess);
		BoundComponent->OnFailure.RemoveDynamic(this, &UInputChallengeWidget::HandleFailure);
	}

	Super::NativeDestruct();
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

