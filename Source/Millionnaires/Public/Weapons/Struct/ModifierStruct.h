#pragma once

#include "Weapons/Enum/ModifierEnum.h"
#include "CoreMinimal.h"
#include "ModifierStruct.generated.h"

USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModifierType Type = EModifierType::Multiplicative;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Source;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tag;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = -1.f; 
    
	float CreationTime = 0.f;
    
	FStatModifier() {}
    
	FStatModifier(float InValue, EModifierType InType, FText InSource = FText(), FName InTag = NAME_None, float InDuration = -1.f)
		: Value(InValue), Type(InType), Source(InSource), Tag(InTag), Duration(InDuration) {}
};

USTRUCT(BlueprintType)
struct FGameplayAttribute
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	float BaseValue = 0.f;
    
	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	TArray<FStatModifier> Modifiers;
    
	float GetCurrentValue() const
	{
		float Value = BaseValue;
		
		for (const FStatModifier& Mod : Modifiers)
		{
			if (Mod.Type == EModifierType::Additive)
				Value += Mod.Value;
		}
		
		for (const FStatModifier& Mod : Modifiers)
		{
			if (Mod.Type == EModifierType::Multiplicative)
				Value *= Mod.Value;
		}
        
		return FMath::Max(0.f, Value);
	}
    
	void AddModifier(const FStatModifier& Modifier)
	{
		if (Modifier.Tag != NAME_None)
		{
			RemoveModifier(Modifier.Tag);
		}
		Modifiers.Add(Modifier);
	}
    
	void RemoveModifier(FName Tag)
	{
		Modifiers.RemoveAll([Tag](const FStatModifier& M) { return M.Tag == Tag; });
	}
    
	void ClearModifiers()
	{
		Modifiers.Empty();
	}
    
	void CleanupExpired(float CurrentTime)
	{
		Modifiers.RemoveAll([CurrentTime](const FStatModifier& M)
		{
			if (M.Duration < 0.f) return false;
			return (CurrentTime - M.CreationTime) >= M.Duration;
		});
	}
};