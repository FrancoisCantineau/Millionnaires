// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "TraversalInterface.h"
#include "AnimNotify_TraversalEvent.generated.h"

/**
 * 
 */
UCLASS()
class TRAVERSALSYSTEM_API UAnimNotify_TraversalEvent : public UAnimNotify
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETraversalNotifyType EventType = ETraversalNotifyType::None;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
