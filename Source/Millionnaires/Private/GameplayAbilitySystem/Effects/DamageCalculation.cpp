// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Effects/DamageCalculation.h"

UDamageCalculation::UDamageCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    
    if (!TargetASC || !SourceASC)
    {
        return;
    }
    
    float BaseDamage = 0.f;
    Spec.GetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(FName("Damage.Calculation")), 
        false, 
        BaseDamage);
    
    if (BaseDamage == 0.f)
    {
        BaseDamage = 10.f; 
    }

    UE_LOG(LogTemp, Warning, TEXT("Base Damage: %f"), BaseDamage);
    
    FGameplayTag FrozenTag = FGameplayTag::RequestGameplayTag(FName("State.Freeze.Frozen"));
    if (TargetASC->HasMatchingGameplayTag(FrozenTag))
    {
        BaseDamage *= 2.0f;
        
    }
    
    FGameplayTag OnFireTag = FGameplayTag::RequestGameplayTag(FName("Status.OnFire"));
    if (TargetASC->HasMatchingGameplayTag(OnFireTag))
    {
        BaseDamage += 20.f;
    }
    
    FGameplayTag PoisonedTag = FGameplayTag::RequestGameplayTag(FName("Status.Poisoned"));
    if (TargetASC->HasMatchingGameplayTag(PoisonedTag))
    {
        BaseDamage *= 1.5f; 
    }

    if (BaseDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            DamageStatics().HealthProperty,
            EGameplayModOp::Additive,
            -BaseDamage));
    }
}