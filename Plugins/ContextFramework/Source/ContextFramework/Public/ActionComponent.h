// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"


struct FActiveContext;
class UContextDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONTEXTFRAMEWORK_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void HandleContextAdded(const FActiveContext& ContextData);
	void HandleContextRemoved(const FActiveContext& ContextData);

	void BlockAction(const FGameplayTag& ActionTag);
	void UnblockAction(const FGameplayTag& ActionTag);
	
	
	UPROPERTY()
	TMap<FGameplayTag, int32> BlockCounters;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool CanPerform(const FGameplayTag& ActionTag) const;
		
};

