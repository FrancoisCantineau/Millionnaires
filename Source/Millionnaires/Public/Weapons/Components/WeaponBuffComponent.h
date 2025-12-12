// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Implements the logic for buffs/debuffs of the weapon.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Weapons/Struct/ModifierStruct.h"

#include "WeaponBuffComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UWeaponBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UWeaponBuffComponent();
    
    //** Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Attributes")
    FGameplayAttribute Damage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Attributes")
    FGameplayAttribute FireRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Attributes")
    FGameplayAttribute Range;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Attributes")
    FGameplayAttribute CriticalChance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Attributes")
    FGameplayAttribute CriticalMultiplier;
    
    //** Public Functions */
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void AddModifierToAttribute(FName AttributeName, const FStatModifier& Modifier);
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void RemoveModifierFromAttribute(FName AttributeName, FName ModifierTag);
    
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void ClearAllModifiers();
    

    FGameplayAttribute* GetAttributeByName(FName AttributeName);
    
    // Helpers
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetDamage() const { return Damage.GetCurrentValue(); }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetFireRate() const { return FireRate.GetCurrentValue(); }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetRange() const { return Range.GetCurrentValue(); }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetCriticalChance() const { return CriticalChance.GetCurrentValue(); }
    
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetCriticalMultiplier() const { return CriticalMultiplier.GetCurrentValue(); }
    
    // Initialize depuis un DataAsset
    void InitializeFromData(float InDamage, float InFireRate, float InRange);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    void CleanupExpiredModifiers();
    
    // Map pour accès dynamique par nom
    TMap<FName, FGameplayAttribute*> AttributeMap;
    
    void BuildAttributeMap();
};
