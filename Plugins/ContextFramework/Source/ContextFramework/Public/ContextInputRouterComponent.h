// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"         
#include "Data/ContextInputMappingDataAsset.h" 
#include "ContextInputRouterComponent.generated.h"


	class UInputAction;
	struct FInputActionInstance;

	UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
	class CONTEXTFRAMEWORK_API UContextInputRouterComponent : public UActorComponent
	{
		GENERATED_BODY()

	public:	
		// Sets default values for this component's properties
		UContextInputRouterComponent();


		AActor* CurrentReceiver = nullptr;

	protected:
		// Called when the game starts
		virtual void BeginPlay() override;

		

		TMap<TObjectPtr<UInputAction>, FGameplayTag> ActionToTagMap;
		
	public:	
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		void HandleInputReceived(const FInputActionInstance& Instance);

		UPROPERTY(EditDefaultsOnly)
		UContextInputMappingDataAsset* InputMappingDataAsset;
		
	};
