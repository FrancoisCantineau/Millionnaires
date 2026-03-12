// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Light/LightBase.h"

// Sets default values
ALightBase::ALightBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALightBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FLightConfigStruct ALightBase::GetActiveConfig() const
{
	if (bOverrideConfig)
		return OverrideConfig;
	
	if (LampConfigAsset)
		return LampConfigAsset->LightConfig;
	
	return FLightConfigStruct();
}

void ALightBase::OnPowerFailure_Implementation()
{
	bIsPowered = false;
	PowerLight(false);
}

void ALightBase::OnPowerRestore_Implementation()
{
	bIsPowered = true;
	PowerLight(true);
}

