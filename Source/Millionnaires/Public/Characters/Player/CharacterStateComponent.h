// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CharacterStateComponent.generated.h"


struct FContextTagChange;

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	Default,
	Crouch,
	Ladder,
	Crawl,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateTagChanged, FGameplayTag, Tag, bool, bAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MILLIONNAIRES_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStateComponent();

	UFUNCTION(BlueprintCallable)
	bool HasState(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void AddState(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveState(FGameplayTag Tag);
	
	UFUNCTION(BlueprintCallable)
	ELocomotionState GetCurrentState() const{return CurrentState;};

	UPROPERTY(BlueprintAssignable)
	FOnStateTagChanged OnStateTagChanged;

protected:

	virtual void BeginPlay() override;

	void HandleTagChanged(const FContextTagChange& Change);

	void ResolveState();

private:

	UPROPERTY()
	FGameplayTagContainer ActiveStates;

	UPROPERTY()
	
	class UContextComponent* ContextComponent;

	UPROPERTY()
	ELocomotionState CurrentState;
};
