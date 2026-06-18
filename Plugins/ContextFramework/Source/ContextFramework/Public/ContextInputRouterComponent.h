// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"         
#include "Data/ContextInputMappingDataAsset.h" 
#include "ContextInputRouterComponent.generated.h"


struct FActiveContext;
class UInputAction;
class UContextComponent;
struct FInputActionInstance;

	UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
	class CONTEXTFRAMEWORK_API UContextInputRouterComponent : public UActorComponent
	{
		GENERATED_BODY()

	public:	
		// Sets default values for this component's properties
		UContextInputRouterComponent();


		UPROPERTY()
		TObjectPtr<UObject> CurrentReceiver;

	protected:
		// Called when the game starts
		virtual void BeginPlay() override;

		UPROPERTY()
		const UContextInputMappingDataAsset* ActiveMapping; 

		UPROPERTY()
		UContextComponent* CachedContextComponent = nullptr;
		
		UPROPERTY()
		TObjectPtr<UContextComponent> ContextSource;

		TMap<TObjectPtr<const UInputAction>, FGameplayTag> ActionToTagMap;

		void OnContextAdded(const FActiveContext& Context);
		void OnContextRemoved(const FActiveContext& Context);

		void BuildActionMap();
		
		UPROPERTY()
		TObjectPtr<UContextComponent> ContextComponent;
		
	public:	
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		void HandleInputReceived(const FInputActionInstance& Instance);

		UPROPERTY(EditDefaultsOnly)
		UContextInputMappingDataAsset* InputMappingDataAsset;
		
		void Initialize(UContextComponent* InContext);
		
	};
