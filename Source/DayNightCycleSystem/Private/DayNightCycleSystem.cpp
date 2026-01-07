#include "DayNightCycleSystem.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"

#pragma region Initialization

ADayNightCycleSystem::ADayNightCycleSystem()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
    SunLight->SetupAttachment(Root);
    SunLight->SetIntensity(10.0f);
    SunLight->SetLightColor(FLinearColor::White);
    SunLight->bAffectsWorld = true;
    SunLight->bAtmosphereSunLight = true;
    SunLight->SetMobility(EComponentMobility::Movable);

    SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
    SkyLight->SetupAttachment(Root);
    SkyLight->SetIntensity(1.0f);
    SkyLight->bRealTimeCapture = true;
    SkyLight->SetMobility(EComponentMobility::Movable);
}

void ADayNightCycleSystem::BeginPlay()
{
    Super::BeginPlay();
    
    if (SunLight)
    {
        SunLight->SetMobility(EComponentMobility::Movable);
        SunLight->bAtmosphereSunLight = true;
    }
    
    if (SkyLight)
    {
        SkyLight->SetMobility(EComponentMobility::Movable);
        SkyLight->RecaptureSky();
    }
    
    UpdateLighting();
}

void ADayNightCycleSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bAutoAdvanceTime)
        return;

    CurrentTimeOfDay += (24.0f / DayDurationInSeconds) * DeltaTime;

    if (CurrentTimeOfDay >= 24.0f)
    {
        CurrentTimeOfDay -= 24.0f;
        CurrentDay++;
    }

    static int LastLoggedHour = -1;
    int CurrentHour = FMath::FloorToInt(CurrentTimeOfDay);
    
    if (CurrentHour != LastLoggedHour)
    {
        LastLoggedHour = CurrentHour;
        FString Period = IsDay() ? TEXT("JOUR") : TEXT("NUIT");
    }

    UpdateLighting();
}

#pragma endregion 

#pragma region Lighting Update

/*
 * Update the lighting based on the current time of day.
 */
void ADayNightCycleSystem::UpdateLighting() const
{
    if (!SunLight || !SkyLight)
    {
        return;
    }

    float SunAngle = ((CurrentTimeOfDay - 6.0f) / 24.0f) * 360.0f;
    SunLight->SetWorldRotation(FRotator(SunAngle, 0.0f, 0.0f));

    float SunIntensity, SkyIntensity;
    FLinearColor SunColor;

    if (CurrentTimeOfDay >= 5.0f && CurrentTimeOfDay < 7.0f)
    {
        float Alpha = (CurrentTimeOfDay - 5.0f) / 2.0f;
        SunIntensity = FMath::Lerp(SunIntensityNight, SunIntensityDay, Alpha);
        SkyIntensity = FMath::Lerp(SkyLightIntensityNight, SkyLightIntensityDay, Alpha);
        SunColor = FMath::Lerp(SunColorNight, SunColorSunrise, Alpha);
    }
    else if (CurrentTimeOfDay >= 7.0f && CurrentTimeOfDay < 17.0f)
    {
        SunIntensity = SunIntensityDay;
        SkyIntensity = SkyLightIntensityDay;
        SunColor = SunColorDay;
    }
    else if (CurrentTimeOfDay >= 17.0f && CurrentTimeOfDay < 19.0f)
    {
        float Alpha = (CurrentTimeOfDay - 17.0f) / 2.0f;
        SunIntensity = FMath::Lerp(SunIntensityDay, SunIntensityNight, Alpha);
        SkyIntensity = FMath::Lerp(SkyLightIntensityDay, SkyLightIntensityNight, Alpha);
        SunColor = FMath::Lerp(SunColorSunrise, SunColorNight, Alpha);
    }
    else
    {
        SunIntensity = SunIntensityNight;
        SkyIntensity = SkyLightIntensityNight;
        SunColor = SunColorNight;
    }

    SunLight->SetIntensity(SunIntensity);
    SunLight->SetLightColor(SunColor);
    SkyLight->SetIntensity(SkyIntensity);
}

/*
 * Set the current time of day and update lighting.
 */
void ADayNightCycleSystem::SetTimeOfDay(float NewTime)
{
    CurrentTimeOfDay = FMath::Clamp(NewTime, 0.0f, 24.0f);
    UpdateLighting();
}

/*
 * Get the current time as a formatted string.
 */
FString ADayNightCycleSystem::GetTimeString() const
{
    int32 Hours = FMath::FloorToInt(CurrentTimeOfDay);
    int32 Minutes = FMath::FloorToInt((CurrentTimeOfDay - Hours) * 60.0f);
    
    return FString::Printf(TEXT("Jour %d - %02d:%02d"), CurrentDay, Hours, Minutes);
}

#pragma endregion