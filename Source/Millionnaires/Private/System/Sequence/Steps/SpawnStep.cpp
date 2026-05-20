// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Sequence/Steps/SpawnStep.h"
#include "System/Sequence/Data//SpawnStepData.h"
#include "EngineUtils.h"

void USpawnStep::Init(USequenceStepData* Data, USequenceContext* Context)
{
	Super::Init(Data, Context);

	SpawnData = Cast<USpawnStepData>(Data);
	CachedContext = Context;
}

void USpawnStep::Start(USequenceContext* Context)
{
	Super::Start(Context);

	if (!SpawnData || !SpawnData->ActorClass)
	{
		OnFinished.Broadcast();
		return;
	}

	UWorld* World = Context->World;
	if (!World)
	{
		OnFinished.Broadcast();
		return;
	}
	AActor* Found = nullptr;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->ActorHasTag(SpawnData->SpawnPointTag))
		{
			Found = *It;
			break;
		}
	}

	if (!Found)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnStep: Actor with tag not found"));
		OnFinished.Broadcast();
		return;
	}

	FTransform SpawnTransform = Found->GetActorTransform();
	SpawnTransform.AddToTranslation(SpawnData->Offset);

	World->SpawnActor<AActor>(
		SpawnData->ActorClass,
		SpawnTransform
	);

	OnFinished.Broadcast();
}
