// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ContextComponent.generated.h"


struct FActiveContext;
class UContextDataAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextAdded, const FActiveContext&);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContextRemoved,const FActiveContext&);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONTEXTFRAMEWORK_API UContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContextComponent();

	void AddContext(const FActiveContext& ContextData);

	void RemoveContext(const FActiveContext& ContextData);

	bool HasContext(UContextDataAsset* ContextData) const;
	
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	TArray<FActiveContext> ActiveContexts;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnContextAdded OnContextAdded;
	FOnContextRemoved OnContextRemoved;

		
};
