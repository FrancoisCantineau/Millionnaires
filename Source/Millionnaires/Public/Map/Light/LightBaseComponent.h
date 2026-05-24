// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightConfigAsset.h"
#include "Components/LightComponent.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Map/Events/IncidentManager.h"
#include "LightBaseComponent.generated.h"

class APointLight;

UCLASS(BlueprintType, Blueprintable)
class MILLIONNAIRES_API ULightBaseComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ULightBaseComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetEmissiveIntensity(float Value);

    // Référence manuelle au PointLight séparé
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Light")
    APointLight* LinkedPointLight;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Light")
	AActor* LinkedLightActor;

    // Le StaticMesh owner (trouvé automatiquement)
    UPROPERTY(BlueprintReadOnly, Category = "Light")
    UStaticMeshComponent* LightMesh;

    // Pointeur vers le LightComponent du LinkedPointLight
    UPROPERTY(BlueprintReadOnly, Category = "Light")
    ULightComponent* LightComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
    ULightConfigAsset* LampConfigAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Flicker")
    UMaterialInterface* FlickerLightFunctionMaterial;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
    bool bOverrideConfig = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp", 
            meta = (EditCondition = "bOverrideConfig"))
    FLightConfigStruct OverrideConfig;
    
    UPROPERTY(BlueprintReadOnly, Category = "Lamp|State")
    bool bIsPowered = true;

    void InitializeLight();

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void FlickerLight();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PowerLight(bool bShouldPower);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lamp")
    FLightConfigStruct GetActiveConfig() const;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Lamp|Power")
    void OnPowerFailure();
    virtual void OnPowerFailure_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Lamp|Power")
    void OnPowerRestore();
    virtual void OnPowerRestore_Implementation();

    UFUNCTION(BlueprintCallable, Category = "Light")
    void TurnOn();
 
    UFUNCTION(BlueprintCallable, Category = "Light")
    void TurnOff();
    
    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetEmergencyMode();
 
    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetNormalMode();

    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetIntensity(float NewIntensity);
 
    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetColor(FLinearColor NewColor);
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Emissive")
    int32 EmissiveMaterialSlot = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Emissive")
    FName EmissiveIntensityParamName = "EmissionMultiplicator";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Zone")
    FGameplayTag ZoneType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light|Zone")
    bool bShouldStartOn = true;

private:
    bool bIsOn = true;
    bool bIsInEmergency = false;
    float PulseTime = 0.f;
    float Intensity = 0.f;
    bool bCancelFlickering = false;
    FLinearColor LightColor;
    
    UPROPERTY()
    UMaterialInstanceDynamic* EmissiveDMI;
    
    FTimerHandle PulseTimerHandle;
    FTimerHandle FlickerTimerHandle;

    void InitEmissiveMaterial();
    void StartFlicker();
    void StopFlicker();
    void TriggerFlicker();
    void UpdatePulse();

    void SubscribeToIncidentManager();
    void UnsubscribeFromIncidentManager();

    UFUNCTION()
    void OnIncidentTriggeredDelegate(FShipIncident Incident);
 
    UFUNCTION()
    void OnIncidentResolvedDelegate(FShipIncident Incident);
};
