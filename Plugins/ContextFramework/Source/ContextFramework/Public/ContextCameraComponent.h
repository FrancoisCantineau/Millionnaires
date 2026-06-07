// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContextCameraComponent.generated.h"


struct FActiveContext;
class UContextDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONTEXTFRAMEWORK_API UContextCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContextCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void HandleContextRemoved(const FActiveContext& Context);

	void HandleContextAdded(const FActiveContext& Context);

	APlayerController* GetPlayerController() const;


	UPROPERTY()
	bool bContextActive = false;

	UPROPERTY()
	FRotator BaseContextRotation;

	UPROPERTY()
	FRotator LastControlRotation;

	UPROPERTY()
	float YawMinOffset;

	UPROPERTY()
	float YawMaxOffset;

	UPROPERTY()
	float PitchMinOffset;

	UPROPERTY()
	float PitchMaxOffset;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
