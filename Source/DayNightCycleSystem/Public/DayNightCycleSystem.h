#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DayNightCycleSystem.generated.h"

/**
 * ADayNightCycleSystem
 * 
 * System to manage day/night cycle, updating sun and sky light based on time of day.
 */
UCLASS()
class DAYNIGHTCYCLESYSTEM_API ADayNightCycleSystem : public AActor
{
    GENERATED_BODY()

public:    
    
    ADayNightCycleSystem();

protected:
    
    virtual void BeginPlay() override;

public:   
    
    virtual void Tick(float DeltaTime) override;
    void UpdateLighting() const;

#pragma region System Properties
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UDirectionalLightComponent* SunLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USkyLightComponent* SkyLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UExponentialHeightFogComponent* HeightFog;

#pragma endregion 
    
#pragma region Cycle Properties
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
    float DayDurationInSeconds = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
    float CurrentTimeOfDay = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
    int32 CurrentDay = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle")
    bool bAutoAdvanceTime = true;

#pragma endregion 

#pragma region Visual Parameters

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    float SunIntensityDay = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    float SunIntensityNight = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    FLinearColor SunColorDay = FLinearColor(1.0f, 0.95f, 0.8f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    FLinearColor SunColorSunrise = FLinearColor(1.0f, 0.6f, 0.3f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    FLinearColor SunColorNight = FLinearColor(0.2f, 0.3f, 0.5f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    float SkyLightIntensityDay = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night Cycle|Visual")
    float SkyLightIntensityNight = 0.1f;

#pragma endregion 

#pragma region Fog Parameters
    
    UFUNCTION(BlueprintCallable, Category = "Day/Night Cycle")
    void SetTimeOfDay(float NewTime);

    UFUNCTION(BlueprintCallable, Category = "Day/Night Cycle")
    float GetTimeOfDay() const { return CurrentTimeOfDay; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night Cycle")
    int32 GetCurrentDay() const { return CurrentDay; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night Cycle")
    FString GetTimeString() const;

    UFUNCTION(BlueprintCallable, Category = "Day/Night Cycle")
    bool IsDay() const { return CurrentTimeOfDay >= 6.0f && CurrentTimeOfDay < 18.0f; }

#pragma endregion
    
private:
    
    float TimeAccumulator = 0.0f;
};