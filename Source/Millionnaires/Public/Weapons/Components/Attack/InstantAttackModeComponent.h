// Fill out your copyright notice in the Description page of Project Settings.

/* 
 * Millionaire Project, 2025
 * Created by:  "Francois"
 * Last Updated by: "Francois"
 * Class: "WeaponBase" Header
 * Notes: Attack mode implementation, focused on instant attack
 */

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Components/Attack/AttackModeComponentBase.h"
#include "InstantAttackModeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MILLIONNAIRES_API UInstantAttackModeComponent : public UAttackModeComponentBase
{
	GENERATED_BODY()

public:
	
	virtual void Attack() override;
};
