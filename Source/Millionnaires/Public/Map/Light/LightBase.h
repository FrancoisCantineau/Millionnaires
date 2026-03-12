// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightConfigAsset.h"
#include "GameFramework/Actor.h"
#include "LightBase.generated.h"

UCLASS()
class MILLIONNAIRES_API ALightBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	ULightConfigAsset* LampConfigAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool bOverrideConfig = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp", 
			  meta = (EditCondition = "bOverrideConfig"))
	FLightConfigStruct OverrideConfig;
	
	UPROPERTY(BlueprintReadOnly, Category = "Lamp|State")
	bool bIsPowered = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FlickerLight();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PowerLight(bool bShouldPower);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lamp")
	FLightConfigStruct GetActiveConfig() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Lamp|Power")
	void OnPowerFailure();
	virtual void OnPowerFailure_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Lamp|Power")
	void OnPowerRestore();
	virtual void OnPowerRestore_Implementation();
};
