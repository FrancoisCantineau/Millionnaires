#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DayNightWidget.generated.h"

UCLASS()
class DAYNIGHTCYCLESYSTEM_API UDayNightWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TimeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DayText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night")
    TSubclassOf<class ADayNightCycleSystem> DayNightSystemClass;

private:
    
    class ADayNightCycleSystem* DayNightSystem;
    void FindDayNightSystem();
};