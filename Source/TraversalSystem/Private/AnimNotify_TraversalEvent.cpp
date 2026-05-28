// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_TraversalEvent.h"

#include "Component/TraversalComponent.h"

void UAnimNotify_TraversalEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UTraversalComponent* Traversal = Owner->FindComponentByClass<UTraversalComponent>())
	{
		Traversal->OnTraversalNotify(EventType);
	}
}
