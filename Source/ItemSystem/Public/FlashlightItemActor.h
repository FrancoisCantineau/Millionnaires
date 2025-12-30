#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "Components/SpotLightComponent.h"
#include "FlashlightItemActor.generated.h"

/**
 * Special item actor for flashlight that shows visual light in world
 */
UCLASS()
class ITEMSYSTEM_API AFlashlightItemActor : public AItemActor
{
	GENERATED_BODY()

public:

	AFlashlightItemActor();

	/** Light component for visual representation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
	USpotLightComponent* SpotLightComponent;

	/** Current battery charge for this flashlight instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BatteryCharge = 100.0f;

protected:

	virtual void BeginPlay() override;
};