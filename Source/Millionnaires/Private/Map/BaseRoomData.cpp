// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/BaseRoomData.h"
#include "UObject/ObjectSaveContext.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseRoomData::ABaseRoomData()
{
	PrimaryActorTick.bCanEverTick = false;
 
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

}

// Called when the game starts or when spawned
void ABaseRoomData::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("REAL RoomData BeginPlay: %s (%p)"),
	*GetName(),
	this);

	DebugName = FString::Printf(TEXT("Room_%d"), GetUniqueID());
}

// Called every frame
void ABaseRoomData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FDoorSlotData> ABaseRoomData::GetFreeDoors() 
{
	TArray<FDoorSlotData> FreeDoors;

	for (FDoorSlotData& Data : DoorSlots)
	{
		if (IsValid(Data.Door) && !Data.Door->bIsUsed)
		{
			FreeDoors.Add(Data);
		}
	}

	return FreeDoors;
}

FDoorSlotData ABaseRoomData::GetRandomFreeDoor() 
{
	TArray<FDoorSlotData> FreeDoors = GetFreeDoors();

	if (FreeDoors.IsEmpty())
		return FDoorSlotData();

	return FreeDoors[FMath::RandRange(0, FreeDoors.Num() - 1)];
}

void ABaseRoomData::MarkDoorAsUsed(ABaseDoorSlot* Door)
{
	if (!IsValid(Door)) return;
	Door->bIsUsed = true;
}

void ABaseRoomData::RefreshDoors()
{
#if WITH_EDITOR
	Modify();
#endif

	DoorSlots.Empty();

	ULevel* Level = GetLevel();
	if (!Level) return;

	for (AActor* Actor : Level->Actors)
	{
		ABaseDoorSlot* Door = Cast<ABaseDoorSlot>(Actor);
		if (!IsValid(Door)) continue;

		FDoorSlotData Data;
		Data.Door = Door;
		Data.Transform = Door->GetActorTransform();

		DoorSlots.Add(Data);
	}

#if WITH_EDITOR
	MarkPackageDirty();
#endif
}

void ABaseRoomData::CacheDoorTransforms(AActor* Excluded)
{/*
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		return;
	}

#if WITH_EDITOR
	Modify();
#endif
	
	DoorSlots.Empty();
 
	ULevel* Level = GetLevel();
	if (!Level) return;
 
	for (AActor* Actor : Level->Actors)
	{
		if (Actor == Excluded) continue;
 
		ABaseDoorSlot* Door = Cast<ABaseDoorSlot>(Actor);
		if (IsValid(Door))
		{
			FDoorSlotData Data;
			Data.Door = Door;
			Data.Transform = Door->GetActorTransform();	
			DoorSlots.Add(Data);
		}
	}
#if WITH_EDITOR
	MarkPackageDirty();
#endif*/
}

