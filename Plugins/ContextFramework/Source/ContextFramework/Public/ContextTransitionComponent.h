// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ContextStructData.h"
#include "ContextTransitionComponent.generated.h"

class UContextComponent;
class APlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONTEXTFRAMEWORK_API UContextTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContextTransitionComponent();

	void RequestContextExit(FContextHandle Handle, UAnimMontage* ExitMontage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnEnterMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnEnterMontageBlendingOut(UAnimMontage*, bool bInterrupted);

	void OnExitMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	FContextHandle PendingExitHandle;
	void OnExitMontageBlendingOut(UAnimMontage*, bool bInterrupted);

	void PlayExitMontage(UAnimMontage* Montage);

	
	UPROPERTY()
	TObjectPtr<UContextComponent> ContextComponent;

	UFUNCTION()
	void HandleContextAdded(const FActiveContext& Context);

	UFUNCTION()
	void HandleContextRemoved(const FActiveContext& Context);

	void ApplyViewTarget(const FActiveContext& Context);
	void ApplySnapTarget(const FActiveContext& Context);

	void TickTransition(float DeltaTime);
	void TickComponent(float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction);

	APlayerController* GetPlayerController() const;
	
	void PlayEnterMontage(const FActiveContext& Context);
	
	void StartTransition(const FActiveContext& Context);

	void OnTransitionApproachFinished();

	void EndTransition();

	FActiveContext CurrentContext;


	//MOVE//
	FTimerHandle TransitionTimer;

	FTransform StartTransform;
	FTransform TargetTransform;

	float TransitionAlpha = 0.f;
	float TransitionDuration = 0.25f;
	bool bIsTransitioning = false;

public:	
	// Called every frame
	
		
};
