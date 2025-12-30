#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/DataTable.h"

#include "FlashlightEquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryChanged, float, NewPercentage);

/**
 * Component attached to character that manages equipped flashlight
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UFlashlightEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    
    UFlashlightEquipmentComponent();

    /** Event when battery level changes */
    UPROPERTY(BlueprintAssignable, Category = "Flashlight")
    FOnBatteryChanged OnBatteryChanged;

    /** Maximum battery charge (0-100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    float MaxBatteryCharge = 100.0f;

    /** Current battery charge */
    UPROPERTY(BlueprintReadOnly, Category = "Flashlight")
    float CurrentBatteryCharge = 100.0f;

    /** Battery drain rate per second */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    float DrainRate = 1.667f; // 100/60 seconds = 1 minute

    /** Maximum light intensity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    float MaxIntensity = 5000.0f;

    /** Minimum light intensity (when battery is almost empty) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    float MinIntensity = 500.0f;

    /** Reference to the actual light component */
    UPROPERTY(BlueprintReadOnly, Category = "Flashlight")
    USpotLightComponent* SpotLightComponent;

    /** Is the flashlight currently on? */
    UPROPERTY(BlueprintReadWrite, Category = "Flashlight")
    bool bIsOn = false;

    /** Is a flashlight currently equipped? */
    UPROPERTY(BlueprintReadOnly, Category = "Flashlight")
    bool bIsEquipped = false;

    /** Handle to the equipped flashlight item */
    UPROPERTY(BlueprintReadOnly, Category = "Flashlight")
    FDataTableRowHandle EquippedFlashlightHandle;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override;

    /** Equip a flashlight item */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void EquipFlashlight(const FDataTableRowHandle& FlashlightItemHandle, float InitialBatteryCharge = 100.0f);

    /** Unequip the current flashlight */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void UnequipFlashlight();

    /** Recharge the battery */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void RechargeBattery(float Amount, bool bFullRecharge = false);

    /** Toggle flashlight on/off */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void ToggleFlashlight();

    /** Turn flashlight on */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void TurnOn();

    /** Turn flashlight off */
    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void TurnOff();

    /** Get battery percentage */
    UFUNCTION(BlueprintPure, Category = "Flashlight")
    float GetBatteryPercentage() const { return CurrentBatteryCharge; }

    /** Check if flashlight is equipped */
    UFUNCTION(BlueprintPure, Category = "Flashlight")
    bool IsFlashlightEquipped() const { return bIsEquipped; }

protected:
    
    virtual void BeginPlay() override;

private:
    
    void UpdateLightIntensity();
    void DrainBattery(float DeltaTime);
    void CreateLightComponent();
};