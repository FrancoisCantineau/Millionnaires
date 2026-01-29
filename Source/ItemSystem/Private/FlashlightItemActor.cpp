#include "FlashlightItemActor.h"

AFlashlightItemActor::AFlashlightItemActor()
{
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLightComponent->SetupAttachment(MeshComponent);
	SpotLightComponent->SetIntensity(3000.0f);
	SpotLightComponent->SetOuterConeAngle(45.0f);
	SpotLightComponent->SetAttenuationRadius(2000.0f);
	SpotLightComponent->SetVisibility(false);
}

void AFlashlightItemActor::BeginPlay()
{
	Super::BeginPlay();
}