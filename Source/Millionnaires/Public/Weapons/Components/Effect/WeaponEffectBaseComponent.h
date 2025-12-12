// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponEffectBaseComponent.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UWeaponEffectBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UWeaponEffectBaseComponent();

	UFUNCTION()
	virtual void ApplyEffect(const FHitResult& Hit, AActor* Instigator);
protected:
	
	virtual void BeginPlay() override;

	

	
public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
