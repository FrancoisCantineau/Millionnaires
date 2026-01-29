#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DayNightWidget.generated.h"

class UTextBlock;
class ADaySequenceActor;

/**
 * UDayNightWidget
 * 
 * - widget for displaying the current time of day and day number
 * - updates based on a DaySequenceActor in the level
 */
UCLASS()
class DAYNIGHTCYCLESYSTEM_API UDayNightWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DayText;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (ClampMin = "1.0"))
    float DayDurationInSeconds = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (ClampMin = "0.0", ClampMax = "24.0"))
    float DayStartHour = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (ClampMin = "0.0", ClampMax = "24.0"))
    float DayEndHour = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
    FLinearColor DayTextColor = FLinearColor::Black;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
    FLinearColor NightTextColor = FLinearColor::White;

private:
    
    /* Find and cache the DaySequenceActor in the level */
    void FindDaySequenceActor();

    /* Get the current time of day in hours */
    float GetCurrentTimeOfDay() const;

    /* Get the current day number */
    int32 GetCurrentDay() const;

    /* Check if it's currently day time */
    bool IsDay() const;

    /* Update the displayed time and day */
    void UpdateDisplay();

    UPROPERTY()
    ADaySequenceActor* DaySequenceActor;

    float TotalElapsedTime = 0.0f;
};