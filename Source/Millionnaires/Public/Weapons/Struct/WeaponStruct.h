#pragma once

#include "CoreMinimal.h"
#include "WeaponStruct.generated.h"


USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseAttackSpeed = 1.f; 


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FinalAttackSpeed = 1.f; 


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage = 10.f;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FinalDamage = 10.f;
};