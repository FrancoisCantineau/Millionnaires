// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/DamageData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DamageStatic.generated.h"

/**
 * 
 */
UCLASS()
class MILLIONNAIRES_API UDamageStatic : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Combat|Damage")
	static void ApplyImpactDamageToActor(const FDamageData& DamageData, AActor* TargetActor, FVector ImpactPoint);

	UFUNCTION(BlueprintCallable, Category="Combat|Damage")
	static void ApplyRadialDamage(const FDamageData& DamageData, FVector OriginPoint);
};
