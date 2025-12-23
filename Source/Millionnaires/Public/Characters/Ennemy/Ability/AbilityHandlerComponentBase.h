// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "Components/ActorComponent.h"
#include "AbilityHandlerComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UAbilityHandlerComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Ability")
	TArray<TSubclassOf<UAbilityBase>> AbilityClasses;

protected:

	UAbilityHandlerComponentBase();
	
	UPROPERTY()
	TArray<UAbilityBase*> Abilities;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	bool UseAbilityByIndex(int32 Index, AActor* Target);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
