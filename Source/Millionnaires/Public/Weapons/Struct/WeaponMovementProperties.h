// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "AttackModeComponentBase" Header
 * Notes: Struct for movement datas.
 */

#pragma once

#include "CoreMinimal.h"
#include "WeaponMovementProperties.generated.h"

USTRUCT(BlueprintType)
struct FWeaponMovementProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OrientRotationToMovement= false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseControllerDesiredRotation = false;
};
    
