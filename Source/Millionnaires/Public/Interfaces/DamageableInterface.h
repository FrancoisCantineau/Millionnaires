#pragma once

#include "CoreMinimal.h"
#include "DamageableInterface.generated.h"


UINTERFACE(Blueprintable)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class IDamageableInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyDamage(
		float Damage,
		AActor* DamageCauser
	);
};