#pragma once

#include "CoreMinimal.h"
#include "LightConfigStruct.generated.h"


class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FLampFlickerConfig
{
    GENERATED_BODY()
 
    // Active ou désactive le flicker
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
    bool bFlickerEnabled = false;
 
    // Fréquence de base entre chaque tentative de flicker (secondes)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.1"))
    float FlickerFrequency = 2.0f;
 
    // Variance aléatoire sur la fréquence (+/- cette valeur)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0"))
    float FlickerFrequencyVariance = 0.5f;
 
    // Probabilité que le flicker se déclenche (0.0 à 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0", ClampMax = "1.0"))
    float FlickerChance = 0.3f;
 
    // Durée du flicker (secondes)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0"))
    float FlickerDuration = 0.1f;
 
    // Intensité minimale pendant le flicker (0.0 = éteinte)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0", ClampMax = "1.0"))
    float FlickerMinIntensityRatio = 0.0f;
};
 
USTRUCT(BlueprintType)
struct FLampSoundConfig
{
    GENERATED_BODY()
 
    // Active ou désactive le son
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    bool bSoundEnabled = false;
 
    // Son joué en boucle quand la lampe est allumée
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled"))
    TSoftObjectPtr<USoundBase> AmbientSound = nullptr;
 
    // Son joué lors d'un flicker
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled"))
    TSoftObjectPtr<USoundBase> FlickerSound = nullptr;
 
    // Volume du son ambiant
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled", ClampMin = "0.0", ClampMax = "2.0"))
    float AmbientVolume = 0.5f;
 
    // Volume du son de flicker
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled", ClampMin = "0.0", ClampMax = "2.0"))
    float FlickerVolume = 1.0f;
};
 
USTRUCT(BlueprintType)
struct FLampLightConfig
{
    GENERATED_BODY()
 
    // Intensité de la lumière (Lumen)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (ClampMin = "0.0"))
    float Intensity = 1000.0f;
 
    // Couleur de la lumière
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    FLinearColor Color = FLinearColor::White;
 
    // Rayon d'atténuation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (ClampMin = "0.0"))
    float AttenuationRadius = 500.0f;
 
    // Température de couleur (si bUseTemperature est true)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
    bool bUseTemperature = false;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (EditCondition = "bUseTemperature", ClampMin = "1700.0", ClampMax = "12000.0"))
    float Temperature = 6500.0f;
};
 
USTRUCT(BlueprintType)
struct FLampNiagaraConfig
{
    GENERATED_BODY()
 
    // Active ou désactive les sparks Niagara
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    bool bSparkEnabled = false;
 
    // Le système Niagara pour les sparks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (EditCondition = "bSparkEnabled"))
    TSoftObjectPtr<UNiagaraSystem> SparkSystem = nullptr;
 
    // Scale du système Niagara
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (EditCondition = "bSparkEnabled"))
    FVector SparkScale = FVector(1.0f);
};
 
USTRUCT(BlueprintType)
struct FLampPowerConfig
{
    GENERATED_BODY()
 
    // Cette lampe réagit-elle aux pannes de courant ?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    bool bReactsToPowerFailure = true;
 
    // Lampe de secours : reste allumée lors d'une panne
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    bool bIsEmergencyLight = false;
 
    // Couleur de secours (si lampe d'urgence)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (EditCondition = "bIsEmergencyLight"))
    FLinearColor EmergencyColor = FLinearColor(1.0f, 0.0f, 0.0f); // Rouge par défaut
 
    // Intensité de secours
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (EditCondition = "bIsEmergencyLight", ClampMin = "0.0"))
    float EmergencyIntensity = 500.0f;
};

USTRUCT(Blueprintable)
struct FEmergencyLightConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency")
    FLinearColor EmergencyColor = FLinearColor(1.0f, 0.0f, 0.0f); // Rouge
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency",
        meta = (ClampMin = "0.0"))
    float EmergencyIntensity = 300.0f;
 
    // Pulse sinusoïdal en urgence
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse")
    bool bShouldPulse = false;
 
    // Vitesse du pulse (plus c'est élevé, plus c'est rapide)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.1"))
    float PulseSpeed = 1.0f;
 
    // Intensité minimale du pulse (ratio)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.0", ClampMax = "1.0"))
    float PulseMinIntensityRatio = 0.1f;
 
    // Intensité maximale du pulse (ratio)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.0", ClampMax = "1.0"))
    float PulseMaxIntensityRatio = 1.0f;
 
    // Lampe rotative (gyrophare)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Rotation")
    bool bIsRotatingLight = false;
 
    // Vitesse de rotation en degrés/seconde
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Rotation",
        meta = (EditCondition = "bIsRotatingLight"))
    float RotationSpeed = 180.0f;
 
    // Son d'alarme
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound")
    bool bHasAlertSound = false;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound",
        meta = (EditCondition = "bHasAlertSound"))
    TSoftObjectPtr<USoundBase> AlertSound;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound",
        meta = (EditCondition = "bHasAlertSound", ClampMin = "0.0", ClampMax = "2.0"))
    float AlertVolume = 1.0f;
};

USTRUCT(BlueprintType)
struct FLightConfigStruct
{
    GENERATED_BODY()
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Flicker")
    FLampFlickerConfig Flicker;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Sound")
    FLampSoundConfig Sound;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Light")
    FLampLightConfig Light;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Niagara")
    FLampNiagaraConfig Niagara;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Power")
    FLampPowerConfig Power;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Emergency")
    FEmergencyLightConfig EmergencySettings;
};