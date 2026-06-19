// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Data/ContextStructData.h"
#include "ContextComponent.generated.h"


struct FActiveContext;
class UContextDataAsset;

UENUM()
enum class EContextState : uint8
{
	None,
	Transitioning,
	Active,
	Exiting,
	StartExit,
};


struct FContextTagChange
{
	FGameplayTag Tag;
	bool bAdded;
};



DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextAdded, const FActiveContext&);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextRemoved,const FActiveContext&);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnContextStateChanged, const FActiveContext&, EContextState);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextTagChanged, const FContextTagChange&);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONTEXTFRAMEWORK_API UContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContextComponent();

	FContextHandle AddContext(FActiveContext ContextData);

	bool RemoveContext(FContextHandle ContextHandle);

	bool HasContext(UContextDataAsset* ContextData) const;
	
	const FActiveContext* GetTopContext() const;
	
	UFUNCTION(BlueprintCallable)
	FActiveContext GetTopContextBP() const;

	void SetContextState(EContextState ContextState);

	EContextState GetContextState(){return CurrentState;};
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	TArray<FActiveContext> ActiveContexts;

	void ApplyContextToInput(const FActiveContext& Context);
	
	EContextState CurrentState = EContextState::Active;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnContextAdded OnContextAdded;
	FOnContextRemoved OnContextRemoved;
	FOnContextStateChanged OnContextStateChanged;
	FOnContextTagChanged OnContextTagChanged;

		
};
