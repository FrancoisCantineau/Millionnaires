#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "Combat/Damages/Data/DamageData.h"
#include "WeaponContextStruct.generated.h"

USTRUCT(BlueprintType)
struct FWeaponContextStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* Instigator = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ProjectileCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpreadAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalSpreadAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayEffectSpecHandle DamageSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damages")
	FDamageData DamageData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStopOnFirstHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius = 0.f;

	// FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ImpactCueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ExplosionCueTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> WeaponEffects;
	
};