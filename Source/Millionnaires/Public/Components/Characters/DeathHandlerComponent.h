// Fill out your copyright notice in the Description page of Project Settings.
/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "DeathHandlerComponent " - Header
 * Notes: Component to manage the death. This will execute all the different routines called whenever something dies.
 */


#pragma once

#include "CoreMinimal.h"
#include "Characters/Death/Behaviors/DeathBehaviorObjectBase.h"
#include "Components/ActorComponent.h"
#include "DeathHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UDeathHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeathHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeComponents();
	
	UFUNCTION(BlueprintCallable, Category="Death")
	void ExecuteDeath();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Death")
	TArray<TSubclassOf<UDeathBehaviorObjectBase>> BehaviourClasses;

	UPROPERTY(VisibleInstanceOnly, Transient, Category="Death")
	TArray<UDeathBehaviorObjectBase*> Behaviours;
};
