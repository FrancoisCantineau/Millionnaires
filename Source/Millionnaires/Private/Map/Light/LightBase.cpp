// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Light/LightBase.h"
#include "Map/Events/IncidentManager.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ALightBase::ALightBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightMesh"));
	RootComponent = LightMesh;

}

// Called when the game starts or when spawned
void ALightBase::BeginPlay()
{
	Super::BeginPlay();

	if (!LightComponent) return;
	
	InitializeLight();
	InitEmissiveMaterial();

	if (bShouldStartOn)
	{
		TurnOn();
	}
	else
	{
		TurnOff();
	}
	
	SubscribeToIncidentManager();
}

void ALightBase::InitializeLight()
{
	Intensity =LightComponent->Intensity;
	LightColor = LightComponent->LightColor;
}

// Called every frame
void ALightBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FLightConfigStruct ALightBase::GetActiveConfig() const
{
	if (bOverrideConfig)
		return OverrideConfig;
	
	if (LampConfigAsset)
		return LampConfigAsset->LightConfig;
	
	return FLightConfigStruct();
}

void ALightBase::OnPowerFailure_Implementation()
{
	bIsPowered = false;
	PowerLight(false);
}

void ALightBase::OnPowerRestore_Implementation()
{
	bIsPowered = true;
	PowerLight(true);
}

void ALightBase::TurnOn()
{
	bIsOn = true;
 
	if (IsValid(LightComponent))
	{
		LightComponent->SetVisibility(true);
		LightComponent->SetIntensity(Intensity);
		LightComponent->SetLightColor(LightColor);
	}
 
	SetEmissiveIntensity(1.f);
 
	if (LampConfigAsset->LightConfig.Flicker.bShouldFlicker)
		StartFlicker();
}

void ALightBase::TurnOff()
{
	bIsOn = false;
	StopFlicker();
 
	if (IsValid(LightComponent))
		LightComponent->SetVisibility(false);
 
	SetEmissiveIntensity(0.f);
}

void ALightBase::SetEmergencyMode()
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
		GetWorldTimerManager().SetTimer(
			PulseTimerHandle,
			this,
			&ALightBase::UpdatePulse,
			0.016f, 
			true
		);
	}
}

void ALightBase::SetNormalMode()
{
	bIsInEmergency = false;
	bCancelFlickering = false;
 
	GetWorldTimerManager().ClearTimer(PulseTimerHandle);
	
	if (bIsOn)
		TurnOn();
	else
		TurnOff();

	TurnOn();
}

void ALightBase::SetIntensity(float NewIntensity)
{
	if (IsValid(LightComponent) && bIsOn)
		LightComponent->SetIntensity(NewIntensity);
}

void ALightBase::SetColor(FLinearColor NewColor)
{
	if (IsValid(LightComponent) && bIsOn && !bIsInEmergency)
		LightComponent->SetLightColor(NewColor);
}

void ALightBase::InitEmissiveMaterial()
{
	if (!IsValid(LightMesh)) return;
 
	UMaterialInterface* Mat = LightMesh->GetMaterial(EmissiveMaterialSlot);
	if (!IsValid(Mat)) return;
 
	EmissiveDMI = UMaterialInstanceDynamic::Create(Mat, this);
	LightMesh->SetMaterial(EmissiveMaterialSlot, EmissiveDMI);
}

void ALightBase::SetEmissiveIntensity(float Value)
{
	if (!IsValid(EmissiveDMI)) return;
	EmissiveDMI->SetScalarParameterValue(EmissiveIntensityParamName, Value);
}

void ALightBase::StartFlicker()
{
	if (!IsValid(LampConfigAsset)) return;
	
	float BaseDelay = 1.f / FMath::Max(LampConfigAsset->LightConfig.Flicker.FlickerSpeed, 0.1f);
	float RandomDelay = BaseDelay + FMath::RandRange(
		-LampConfigAsset->LightConfig.Flicker.FlickerFrequencyVariance,
		LampConfigAsset->LightConfig.Flicker.FlickerFrequencyVariance
	);
	RandomDelay = FMath::Max(RandomDelay, 0.05f);
 
	GetWorldTimerManager().SetTimer(
		FlickerTimerHandle,
		this,
		&ALightBase::TriggerFlicker,
		RandomDelay,
		false
	);
}

void ALightBase::StopFlicker()
{
	bCancelFlickering = true;
	GetWorldTimerManager().ClearTimer(FlickerTimerHandle);
	
}

void ALightBase::TriggerFlicker()
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
				GetActorLocation(),
				LampConfigAsset->LightConfig.Sound.FlickerVolume
			);
		}
 

		FTimerHandle TempTimer;
		GetWorldTimerManager().SetTimer(TempTimer, [this]()
		{
			if (IsValid(LightComponent) && bIsOn && !bIsInEmergency)
			{
				LightComponent->SetIntensity(Intensity);
				SetEmissiveIntensity(1.f);
			}
		}, LampConfigAsset->LightConfig.Flicker.FlickerDuration, false);
	}

	if (!bCancelFlickering)
	{
		StartFlicker();
	}
	
}

void ALightBase::UpdatePulse()
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

void ALightBase::SubscribeToIncidentManager()
{
	UIncidentManager* IM = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM)) return;
 
	IM->OnIncidentTriggered.AddDynamic(this, &ALightBase::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.AddDynamic(this, &ALightBase::OnIncidentResolvedDelegate);
}

void ALightBase::UnsubscribeFromIncidentManager()
{
	UIncidentManager* IM = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UIncidentManager>();
	if (!IsValid(IM)) return;
 
	IM->OnIncidentTriggered.RemoveDynamic(this, &ALightBase::OnIncidentTriggeredDelegate);
	IM->OnIncidentResolved.RemoveDynamic(this, &ALightBase::OnIncidentResolvedDelegate);
}

void ALightBase::OnIncidentTriggeredDelegate(FShipIncident Incident)
{
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Light"))) return;
	
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType) return;
 
	SetEmergencyMode();	
}

void ALightBase::OnIncidentResolvedDelegate(FShipIncident Incident)
{
	if (!Incident.AffectedSystems.HasTag(FGameplayTag::RequestGameplayTag("Incident.AffectsSystem.Light"))) return;
	if (Incident.AffectedZone.IsValid() && Incident.AffectedZone != ZoneType) return;
 
	SetNormalMode();
}

