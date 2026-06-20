// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseActors/ClassicTraversalActor.h"

#include "Components/ArrowComponent.h"

void AClassicTraversalActor::MoveForward(UTraversalComponent* TraversalComponent)
{
	Super::MoveForward(TraversalComponent);
	
	if (!TraversalComponent) return;

	const FVector Direction = GetTraversalDirection(TraversalComponent);

	bool bBlocked = TraversalComponent->TraceInDirectionForObstacle(Direction, ObstacleTraceDistance);
	if (bBlocked) return;

	TraversalComponent->UpdateExitPoint(Direction, ExitPointDistance);

	FVector StartTracePosition = GetExitPoint()->GetComponentLocation();
	FVector EndTracePosition = StartTracePosition + GetExitPoint()->GetForwardVector() * ExitTraceDistance;
	
	if (!ExitPoint) return;

	bool bCanExit = TraversalComponent->TraceFromPoint(
		StartTracePosition, EndTracePosition);

	if (!bCanExit)
	{
		TraversalComponent->RequestExitTraversal(GetMontageForContext(1.f, TraversalComponent->GetCurrentHand(), true, false, false, true));
	}
	else
	{
		TraversalComponent->RequestMontage(1.f, false, false, false, false);
	}
}

void AClassicTraversalActor::MoveBackward(UTraversalComponent* TraversalComponent)
{
	Super::MoveBackward(TraversalComponent);
	
	if (!TraversalComponent) return;

	const FVector Direction = -GetTraversalDirection(TraversalComponent);

	bool bBlocked = TraversalComponent->TraceInDirectionForObstacle(Direction, ObstacleTraceDistance);
	if (bBlocked)
	{
		TraversalComponent->RequestExitTraversal(GetMontageForContext(-1.f, TraversalComponent->GetCurrentHand(), true, false, true, false));
		return;
	}

	TraversalComponent->RequestMontage(-1.f, false, false, false, false);
}
