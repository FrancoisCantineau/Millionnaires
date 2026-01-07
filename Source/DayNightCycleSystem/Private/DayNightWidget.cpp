#include "DayNightWidget.h"

#include "DayNightCycleSystem.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDayNightWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FindDayNightSystem();
}

/*
 * Update the widget every frame to reflect the current time and day.
 */
void UDayNightWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!DayNightSystem)
	{
		FindDayNightSystem();
		return;
	}

	if (TimeText && DayText)
	{
		float TimeOfDay = DayNightSystem->GetTimeOfDay();
		int32 Hours = FMath::FloorToInt(TimeOfDay);
		int32 Minutes = FMath::FloorToInt((TimeOfDay - Hours) * 60.0f);
        
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
		TimeText->SetText(FText::FromString(TimeString));

		FString DayString = FString::Printf(TEXT("Jour %d"), DayNightSystem->GetCurrentDay());
		DayText->SetText(FText::FromString(DayString));

		FLinearColor TextColor = DayNightSystem->IsDay() ? 
			FLinearColor::Black : FLinearColor::White;
        
		TimeText->SetColorAndOpacity(FSlateColor(TextColor));
		DayText->SetColorAndOpacity(FSlateColor(TextColor));
	}
}

/*
 * Locate the Day/Night Cycle System actor in the level.
 */
void UDayNightWidget::FindDayNightSystem()
{
	if (DayNightSystem)
		return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADayNightCycleSystem::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		DayNightSystem = Cast<ADayNightCycleSystem>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" Error : No DayNightCycleSystem actor found in the level."));
	}
}