#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "DamageData.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct MILLIONNAIRES_API FDamageData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SourceActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAbilitySystemComponent* SourceASC = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayEffectSpecHandle DamageSpec;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> AdditionalEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Origin = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ImpactCueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ExplosionCueTag;
};