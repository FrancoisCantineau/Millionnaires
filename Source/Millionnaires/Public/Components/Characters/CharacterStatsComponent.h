/*
 * Millionaire Project, 2025
 * Created by:  "0nnen"
 * Last Updated by: "0nnen"
 * Class: "CharacterStatsComponent" - Header
 * Notes: Actor component handling health, hunger and basic character data, driven by CharacterDefinition.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatsComponent.generated.h"

class UCharacterDefinition;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHungerChangedSignature, float, NewHunger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup = (Character), meta = (BlueprintSpawnableComponent))
class MILLIONNAIRES_API UCharacterStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UCharacterStatsComponent();

    UFUNCTION(BlueprintPure, Category = "Stats")
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void InitializeFromDefinition(UCharacterDefinition* NewDefinition);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ModifyHealth(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void ModifyHunger(float Delta);

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHunger() const { return CurrentHunger; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetMaxHunger() const { return MaxHunger; }

    UFUNCTION(BlueprintPure, Category = "Definition")
    FText GetDisplayName() const { return CachedDisplayName; }

    UFUNCTION(BlueprintPure, Category = "Definition")
    UTexture2D* GetPortrait() const { return Portrait; }

    UFUNCTION(BlueprintPure, Category = "Definition")
    UCharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }

protected:

    virtual void BeginPlay() override;

    void ApplyDefinition(UCharacterDefinition* DefinitionToApply);
    void ClampStats();

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "If true, the component will log initialization and stat changes."))
    bool bDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition", meta = (ToolTip = "Data asset providing default stats and display data for this character."))
    TObjectPtr<UCharacterDefinition> CharacterDefinition;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition", meta = (ToolTip = "If true, stats are initialized from CharacterDefinition at BeginPlay."))
    bool bAutoInitializeFromDefinition = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (InlineEditConditionToggle, ToolTip = "If disabled, health is ignored and no death events are fired."))
    bool bUseHealth = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (EditCondition = "bUseHealth", ClampMin = "0.0", ToolTip = "Maximum health points for this actor."))
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (InlineEditConditionToggle, ToolTip = "If enabled, hunger is tracked (usually only for the player)."))
    bool bUseHunger = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (EditCondition = "bUseHunger", ClampMin = "0.0", ToolTip = "Maximum hunger value when hunger is enabled."))
    float MaxHunger = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Runtime", meta = (ToolTip = "Current health value at runtime."))
    float CurrentHealth = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Runtime", meta = (ToolTip = "Current hunger value at runtime."))
    float CurrentHunger = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Definition", meta = (ToolTip = "Display name cached from the CharacterDefinition."))
    FText CachedDisplayName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Definition", meta = (ToolTip = "Portrait texture cached from the CharacterDefinition."))
    TObjectPtr<UTexture2D> Portrait;

public:

    UPROPERTY(BlueprintAssignable, Category = "Stats|Delegates")
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stats|Delegates")
    FOnHungerChangedSignature OnHungerChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stats|Delegates")
    FOnDeathSignature OnDeath;
};
