#pragma once

#include "CoreMinimal.h"
#include "LightConfigStruct.generated.h"


class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FLampFlickerConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker")
    bool bShouldFlicker = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.1"))
    float FlickerSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Flicker")
    float FlickerIntensityMin = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0"))
    float FlickerFrequencyVariance = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0", ClampMax = "1.0"))
    float FlickerChance = 0.3f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker", meta = (EditCondition = "bFlickerEnabled", ClampMin = "0.0"))
    float FlickerDuration = 0.1f;
    
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
    USoundBase* AmbientSound = nullptr;
 
    // Son joué lors d'un flicker
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled"))
    USoundBase* FlickerSound = nullptr;
 
    // Volume du son ambiant
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled", ClampMin = "0.0", ClampMax = "2.0"))
    float AmbientVolume = 0.5f;
 
    // Volume du son de flicker
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (EditCondition = "bSoundEnabled", ClampMin = "0.0", ClampMax = "2.0"))
    float FlickerVolume = 1.0f;
};
 
USTRUCT(BlueprintType)
struct FLampNiagaraConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    bool bSparkEnabled = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (EditCondition = "bSparkEnabled"))
    UNiagaraSystem* SparkSystem = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (EditCondition = "bSparkEnabled"))
    FVector SparkScale = FVector(1.0f);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound")
    bool bHasAlertSound = false;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound",
        meta = (EditCondition = "bHasAlertSound"))
    USoundBase* AlertSound;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Sound",
        meta = (EditCondition = "bHasAlertSound", ClampMin = "0.0", ClampMax = "2.0"))
    float AlertVolume = 1.0f;
};


USTRUCT(Blueprintable)
struct FEmergencyLightConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    bool bReactsToPowerFailure = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    bool bIsEmergencyLight = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency")
    FLinearColor EmergencyColor = FLinearColor(1.0f, 0.0f, 0.0f); // Rouge
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency",
        meta = (ClampMin = "0.0"))
    float EmergencyIntensity = 300.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse")
    bool bShouldPulse = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.1"))
    float PulseSpeed = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.0", ClampMax = "1.0"))
    float PulseMinIntensityRatio = 0.1f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Pulse",
        meta = (EditCondition = "bShouldPulse", ClampMin = "0.0", ClampMax = "1.0"))
    float PulseMaxIntensityRatio = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Rotation")
    bool bIsRotatingLight = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergency|Rotation",
        meta = (EditCondition = "bIsRotatingLight"))
    float RotationSpeed = 180.0f;
};

USTRUCT(BlueprintType)
struct FLightConfigStruct
{
    GENERATED_BODY()
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Flicker")
    FLampFlickerConfig Flicker;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Sound")
    FLampSoundConfig Sound;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Niagara")
    FLampNiagaraConfig Niagara;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp|Emergency")
    FEmergencyLightConfig EmergencySettings;
};