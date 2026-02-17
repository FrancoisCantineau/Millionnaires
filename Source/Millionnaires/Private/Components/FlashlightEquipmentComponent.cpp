#include "Components/FlashlightEquipmentComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/SpotLightComponent.h"

UFlashlightEquipmentComponent::UFlashlightEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f;
}

void UFlashlightEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    CreateLightComponent();
}

void UFlashlightEquipmentComponent::CreateLightComponent()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    USceneComponent* AttachPoint = nullptr;
    
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        TArray<UActorComponent*> Components;
        Character->GetComponents(UCameraComponent::StaticClass(), Components);
        
        if (Components.Num() > 0)
        {
            UCameraComponent* CameraComp = Cast<UCameraComponent>(Components[0]);
            AttachPoint = CameraComp;
        }
        else
        {
            AttachPoint = Character->GetRootComponent();
        }
    }
    else
    {
        AttachPoint = Owner->GetRootComponent();
    }

    if (!AttachPoint)
    {
        return;
    }

    SpotLightComponent = NewObject<USpotLightComponent>(Owner, USpotLightComponent::StaticClass(), TEXT("FlashlightSpot"));
    if (!SpotLightComponent)
    {
        return;
    }
    
    SpotLightComponent->RegisterComponent();
    SpotLightComponent->AttachToComponent(AttachPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
    
    SpotLightComponent->SetIntensity(MaxIntensity);
    SpotLightComponent->SetOuterConeAngle(45.0f);
    SpotLightComponent->SetInnerConeAngle(30.0f);
    SpotLightComponent->SetAttenuationRadius(3000.0f);
    SpotLightComponent->SetVisibility(false);
    SpotLightComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

void UFlashlightEquipmentComponent::EquipFlashlight(const FDataTableRowHandle& FlashlightItemHandle, float InitialBatteryCharge)
{
    if (FlashlightItemHandle.IsNull())
    {
        return;
    }

    EquippedFlashlightHandle = FlashlightItemHandle;
    CurrentBatteryCharge = FMath::Clamp(InitialBatteryCharge, 0.0f, MaxBatteryCharge);
    bIsEquipped = true;
    
    OnBatteryChanged.Broadcast(CurrentBatteryCharge);
}

void UFlashlightEquipmentComponent::UnequipFlashlight()
{
    if (bIsOn)
    {
        TurnOff();
    }

    EquippedFlashlightHandle = FDataTableRowHandle();
    bIsEquipped = false;
}

void UFlashlightEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    static float LogTimer = 0.0f;
    LogTimer += DeltaTime;
    
    if (LogTimer >= 1.0f)
    {
        LogTimer = 0.0f;
    }

    if (!bIsEquipped)
    {
        return;
    }
    
    if (!bIsOn)
    {
        return;
    }

    DrainBattery(DeltaTime);
    UpdateLightIntensity();
}

void UFlashlightEquipmentComponent::DrainBattery(float DeltaTime)
{
    if (CurrentBatteryCharge <= 0.0f)
    {
        CurrentBatteryCharge = 0.0f;
        TurnOff();
        return;
    }

    float OldCharge = CurrentBatteryCharge;
    float DrainAmount = DrainRate * DeltaTime;
    CurrentBatteryCharge = FMath::Max(0.0f, CurrentBatteryCharge - DrainAmount);
    
    if (FMath::FloorToInt(OldCharge) != FMath::FloorToInt(CurrentBatteryCharge))
    {
        OnBatteryChanged.Broadcast(CurrentBatteryCharge);
    }
}

void UFlashlightEquipmentComponent::UpdateLightIntensity()
{
    if (!SpotLightComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateLightIntensity called but SpotLightComponent is NULL"));
        return;
    }
    
    if (!bIsOn)
    {
        return;
    }

    float OldIntensity = SpotLightComponent->Intensity;
    
    float Alpha = CurrentBatteryCharge / MaxBatteryCharge;
    float NewIntensity = FMath::Lerp(MinIntensity, MaxIntensity, Alpha);
    
    SpotLightComponent->SetIntensity(NewIntensity);
    
}

void UFlashlightEquipmentComponent::RechargeBattery(float Amount, bool bFullRecharge)
{
    if (!bIsEquipped)
    {
        return;
    }

    float OldCharge = CurrentBatteryCharge;

    if (bFullRecharge)
    {
        CurrentBatteryCharge = MaxBatteryCharge;
    }
    else
    {
        CurrentBatteryCharge = FMath::Min(MaxBatteryCharge, CurrentBatteryCharge + Amount);
    }
    
    OnBatteryChanged.Broadcast(CurrentBatteryCharge);
    
    if (bIsOn)
    {
        UpdateLightIntensity();
    }
}

void UFlashlightEquipmentComponent::ToggleFlashlight()
{
    
    if (bIsOn)
    {
        TurnOff();
    }
    else
    {
        TurnOn();
    }
}

void UFlashlightEquipmentComponent::TurnOn()
{
    if (!bIsEquipped)
    {
        return;
    }

    if (CurrentBatteryCharge <= 0.0f)
    {
        return;
    }

    if (!SpotLightComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot turn on SpotLightComponent is NULL"));
        return;
    }

    bIsOn = true;
    SpotLightComponent->SetVisibility(true);
    UpdateLightIntensity();
}

void UFlashlightEquipmentComponent::TurnOff()
{
    
    bIsOn = false;
    if (SpotLightComponent)
    {
        SpotLightComponent->SetVisibility(false);
    }
    
}