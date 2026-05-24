// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Light/LightBaseComponent.h"
#include "Map/Events/IncidentManager.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/PointLight.h"
#include "Materials/MaterialInstanceDynamic.h"

ULightBaseComponent::ULightBaseComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void ULightBaseComponent::BeginPlay()
{
    Super::BeginPlay();

    // Récupère le StaticMeshComponent de l'owner
    LightMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();



    if (IsValid(LinkedLightActor))
        LightComponent = LinkedLightActor->FindComponentByClass<ULightComponent>();

    if (LightComponent)
    {
        InitializeLight();
    }

    
    InitEmissiveMaterial();

    if (bShouldStartOn)
        TurnOn();
    else
        TurnOff();

    SubscribeToIncidentManager();
}

void ULightBaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    UnsubscribeFromIncidentManager();
}

void ULightBaseComponent::InitializeLight()
{
    Intensity = LightComponent->Intensity;
    LightColor = LightComponent->LightColor;
}

FLightConfigStruct ULightBaseComponent::GetActiveConfig() const
{
    if (bOverrideConfig)
        return OverrideConfig;

    if (LampConfigAsset)
        return LampConfigAsset->LightConfig;

    return FLightConfigStruct();
}

void ULightBaseComponent::OnPowerFailure_Implementation()
{
    bIsPowered = false;
    PowerLight(false);
}

void ULightBaseComponent::OnPowerRestore_Implementation()
{
    bIsPowered = true;
    PowerLight(true);
}

void ULightBaseComponent::TurnOn()
{
    bIsOn = true;

    if (IsValid(LightComponent))
    {
        LightComponent->SetVisibility(true);
        LightComponent->SetIntensity(Intensity);
        LightComponent->SetLightColor(LightColor);
    }

    SetEmissiveIntensity(1.f);

    if (IsValid(LampConfigAsset) && LampConfigAsset->LightConfig.Flicker.bShouldFlicker)
        StartFlicker();
}

void ULightBaseComponent::TurnOff()
{
    bIsOn = false;
    StopFlicker();

    if (IsValid(LightComponent))
        LightComponent->SetVisibility(false);

    SetEmissiveIntensity(0.f);
}

void ULightBaseComponent::SetEmergencyMode()
{
    if (!IsValid(LampConfigAsset) || !LampConfigAsset->LightConfig.EmergencySettings.bIsEmergencyLight)
    {
        StopFlicker();
        TurnOff();
        return;
    }
    bIsInEmergency = true;

    if (IsValid(LightComponent))
    {
        LightComponent->SetVisibility(true);
        LightComponent->SetLightColor(LampConfigAsset->LightConfig.EmergencySettings.EmergencyColor);
        LightComponent->SetIntensity(LampConfigAsset->LightConfig.EmergencySettings.EmergencyIntensity);

        if (IsValid(FlickerLightFunctionMaterial))
            LightComponent->SetLightFunctionMaterial(FlickerLightFunctionMaterial);
    }

    SetEmissiveIntensity(1.f);

    if (LampConfigAsset->LightConfig.EmergencySettings.bShouldPulse)
    {
        GetWorld()->GetTimerManager().SetTimer(
            PulseTimerHandle,
            this,
            &ULightBaseComponent::UpdatePulse,
            0.016f,
            true
        );
    }
}

void ULightBaseComponent::SetNormalMode()
{
    bIsInEmergency = false;
    bCancelFlickering = false;

    GetWorld()->GetTimerManager().ClearTimer(PulseTimerHandle);

    if (bIsOn)
        TurnOn();
    else
        TurnOff();
}

void ULightBaseComponent::SetIntensity(float NewIntensity)
{
    if (IsValid(LightComponent) && bIsOn)
        LightComponent->SetIntensity(NewIntensity);
}

void ULightBaseComponent::SetColor(FLinearColor NewColor)
{
    if (IsValid(LightComponent) && bIsOn && !bIsInEmergency)
        LightComponent->SetLightColor(NewColor);
}

void ULightBaseComponent::InitEmissiveMaterial()
{
    if (!IsValid(LightMesh)) return;

    UMaterialInterface* Mat = LightMesh->GetMaterial(EmissiveMaterialSlot);
    if (!IsValid(Mat)) return;

    EmissiveDMI = UMaterialInstanceDynamic::Create(Mat, this);
    LightMesh->SetMaterial(EmissiveMaterialSlot, EmissiveDMI);
}

void ULightBaseComponent::SetEmissiveIntensity(float Value)
{
    if (!IsValid(EmissiveDMI)) return;
    EmissiveDMI->SetScalarParameterValue(EmissiveIntensityParamName, Value);
}

void ULightBaseComponent::StartFlicker()
{
    if (!IsValid(LampConfigAsset)) return;

    float BaseDelay = 1.f / FMath::Max(LampConfigAsset->LightConfig.Flicker.FlickerSpeed, 0.1f);
    float RandomDelay = BaseDelay + FMath::RandRange(
        -LampConfigAsset->LightConfig.Flicker.FlickerFrequencyVariance,
        LampConfigAsset->LightConfig.Flicker.FlickerFrequencyVariance
    );
    RandomDelay = FMath::Max(RandomDelay, 0.05f);

    GetWorld()->GetTimerManager().SetTimer(
        FlickerTimerHandle,
        this,
        &ULightBaseComponent::TriggerFlicker,
        RandomDelay,
        false
    );
}

void ULightBaseComponent::StopFlicker()
{
    bCancelFlickering = true;
    GetWorld()->GetTimerManager().ClearTimer(FlickerTimerHandle);
}

void ULightBaseComponent::TriggerFlicker()
{
    if (!IsValid(LampConfigAsset)) return;

    float Roll = FMath::FRand();
    if (Roll < LampConfigAsset->LightConfig.Flicker.FlickerChance)
    {
        float MinIntensity = Intensity * LampConfigAsset->LightConfig.Flicker.FlickerMinIntensityRatio;
        float RandomIntensity = FMath::RandRange(MinIntensity, Intensity);
        float NormalizedIntensity = RandomIntensity / FMath::Max(Intensity, 1.f);

        if (IsValid(LightComponent))
            LightComponent->SetIntensity(RandomIntensity);

        SetEmissiveIntensity(NormalizedIntensity);

        if (LampConfigAsset->LightConfig.Sound.bSoundEnabled && IsValid(LampConfigAsset->LightConfig.Sound.FlickerSound))
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                LampConfigAsset->LightConfig.Sound.FlickerSound,
                GetOwner()->GetActorLocation(), // GetActorLocation() → GetOwner()
                LampConfigAsset->LightConfig.Sound.FlickerVolume
            );
        }

        FTimerHandle TempTimer;
        GetWorld()->GetTimerManager().SetTimer(TempTimer, [this]()
        {
            if (IsValid(LightComponent) && bIsOn && !bIsInEmergency)
            {
                LightComponent->SetIntensity(Intensity);
                SetEmissiveIntensity(1.f);
            }
        }, LampConfigAsset->LightConfig.Flicker.FlickerDuration, false);
    }

    if (!bCancelFlickering)
        StartFlicker();
}

void ULightBaseComponent::UpdatePulse()
{
    if (!IsValid(LampConfigAsset) || !IsValid(LightComponent)) return;

    PulseTime += 0.016f * LampConfigAsset->LightConfig.EmergencySettings.PulseSpeed;
    float PulseValue = (FMath::Sin(PulseTime) * 0.5f) + 0.5f;

    float MinI = LampConfigAsset->LightConfig.EmergencySettings.EmergencyIntensity *
                 LampConfigAsset->LightConfig.EmergencySettings.PulseMinIntensityRatio;
    float MaxI = LampConfigAsset->LightConfig.EmergencySettings.EmergencyIntensity *
                 LampConfigAsset->LightConfig.EmergencySettings.PulseMaxIntensityRatio;

    float NewIntensity = FMath::Lerp(MinI, MaxI, PulseValue);

    LightComponent->SetIntensity(NewIntensity);
    SetEmissiveIntensity(PulseValue);
}

void ULightBaseComponent::SubscribeToIncidentManager()
{
    UIncidentManager* IM = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UIncidentManager>();
    if (!IsValid(IM)) return;

    IM->OnIncidentTriggered.AddDynamic(this, &ULightBaseComponent::OnIncidentTriggeredDelegate);
    IM->OnIncidentResolved.AddDynamic(this, &ULightBaseComponent::OnIncidentResolvedDelegate);
}

void ULightBaseComponent::UnsubscribeFromIncidentManager()
{
    UIncidentManager* IM = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UIncidentManager>();
    if (!IsValid(IM)) return;

    IM->OnIncidentTriggered.RemoveDynamic(this, &ULightBaseComponent::OnIncidentTriggeredDelegate);
    IM->OnIncidentResolved.RemoveDynamic(this, &ULightBaseComponent::OnIncidentResolvedDelegate);
}

void ULightBaseComponent::OnIncidentTriggeredDelegate(FShipIncident Incident)
{
    if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Light"))) return;
    if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType) return;

    SetEmergencyMode();
}

void ULightBaseComponent::OnIncidentResolvedDelegate(FShipIncident Incident)
{
    if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Light"))) return;
    if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType) return;

    SetNormalMode();
}
