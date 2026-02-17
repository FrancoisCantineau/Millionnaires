#include "DayNightWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DaySequenceActor.h"
#include "IDaySequencePlayer.h"

#pragma region Initialize

void UDayNightWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    FindDaySequenceActor();
    
    TotalElapsedTime = 0.0f;
}

/* 
 * Tick 
 * - update the time display based on the DaySequenceActor
 * - if no DaySequenceActor is found, try to find it again
 */
void UDayNightWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!DaySequenceActor)
    {
        FindDaySequenceActor();
        return;
    }

    TotalElapsedTime += InDeltaTime;

    UpdateDisplay();
}

#pragma endregion 

#pragma region DaySequenceActor Management

/*
 * Find DaySequenceActor
 * - search for a DaySequenceActor in the level
 * - cache the reference for future use
 */
void UDayNightWidget::FindDaySequenceActor()
{
    if (DaySequenceActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UDayNightWidget::FindDaySequenceActor: DaySequenceActor already set"));
        return;
    }

    TArray<AActor*> FoundActors;
    
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), 
        ADaySequenceActor::StaticClass(), 
        FoundActors
    );
    
    if (FoundActors.Num() > 0)
    {
        DaySequenceActor = Cast<ADaySequenceActor>(FoundActors[0]);
    }
}

#pragma endregion

#pragma region Time Calculation

/*
 * Get Current Time Of Day
 * - retrieve the current time from the DaySequencePlayer
 * - convert frame time to hours (0-24)
 * - return 12.0 (noon) as default if no valid data
 */
float UDayNightWidget::GetCurrentTimeOfDay() const
{
    if (!DaySequenceActor)
    {
        return 12.0f;
    }

    IDaySequencePlayer* DaySeqPlayer = DaySequenceActor->GetSequencePlayer();
    if (!DaySeqPlayer)
    {
        return 12.0f;
    }

    FQualifiedFrameTime CurrentFrameTime = DaySeqPlayer->GetCurrentTime();
    FQualifiedFrameTime DurationFrameTime = DaySeqPlayer->GetDuration();
    
    FFrameTime CurrentTime = CurrentFrameTime.Time;
    FFrameTime Duration = DurationFrameTime.Time;
    FFrameRate FrameRate = CurrentFrameTime.Rate;

    float CurrentSeconds = CurrentTime.AsDecimal() / FrameRate.AsDecimal();
    float TotalSeconds = Duration.AsDecimal() / FrameRate.AsDecimal();

    if (TotalSeconds <= 0.0f)
    {
        return 12.0f;
    }

    float NormalizedTime = CurrentSeconds / TotalSeconds;
    float TimeOfDay = NormalizedTime * 24.0f;

    while (TimeOfDay >= 24.0f)
        TimeOfDay -= 24.0f;
    while (TimeOfDay < 0.0f)
        TimeOfDay += 24.0f;

    return TimeOfDay;
}

/*
 * Get Current Day
 * - calculate the current day number based on total elapsed time
 * - day 1 starts at 0 seconds
 */
int32 UDayNightWidget::GetCurrentDay() const
{
    int32 Day = FMath::FloorToInt(TotalElapsedTime / DayDurationInSeconds) + 1;
    return FMath::Max(1, Day);
}

/*
 * Is Day
 * - check if the current time is during daytime hours
 * - returns true if between DayStartHour and DayEndHour
 */
bool UDayNightWidget::IsDay() const
{
    float CurrentTime = GetCurrentTimeOfDay();
    return (CurrentTime >= DayStartHour && CurrentTime < DayEndHour);
}

#pragma endregion

#pragma region Display Update

/*
 * Update Display
 * - update the time and day text widgets
 * - change text color based on day/night status
 */
void UDayNightWidget::UpdateDisplay()
{
    float TimeOfDay = GetCurrentTimeOfDay();
    int32 Hours = FMath::FloorToInt(TimeOfDay);
    int32 Minutes = FMath::FloorToInt((TimeOfDay - Hours) * 60.0f);

    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
    TimeText->SetText(FText::FromString(TimeString));

    FString DayString = FString::Printf(TEXT("Jour %d"), GetCurrentDay());
    DayText->SetText(FText::FromString(DayString));

    FLinearColor TextColor = IsDay() ? DayTextColor : NightTextColor;
    TimeText->SetColorAndOpacity(FSlateColor(TextColor));
    DayText->SetColorAndOpacity(FSlateColor(TextColor));
}

#pragma endregion