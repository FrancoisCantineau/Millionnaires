// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/BaseDoorSlot.h"
#include "Map/BaseRoomData.h"

// Sets default values
ABaseDoorSlot::ABaseDoorSlot()
{
	PrimaryActorTick.bCanEverTick = false;
 
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	RootComponent = ArrowComponent;
}

#if WITH_EDITOR

void ABaseDoorSlot::Destroyed()
{
	Super::Destroyed();
#if WITH_EDITOR
	NotifyRoomData(this);
#endif
}
#endif

// Called when the game starts or when spawned
void ABaseDoorSlot::BeginPlay()
{
	Super::BeginPlay();
	
	DebugName = FString::Printf(TEXT("Door_%d"), GetUniqueID());
	
}
#if WITH_EDITOR
void ABaseDoorSlot::NotifyRoomData(AActor* ExcludeActor)
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		return;
	}
	ULevel* Level = GetLevel();
	if (!Level) return;
	
	if (!LevelRoomData)
	{
		for (AActor* Actor : Level->Actors)
		{
			ABaseRoomData* RoomData = Cast<ABaseRoomData>(Actor);
			if (IsValid(RoomData))
			{
				LevelRoomData = RoomData;
				
				break;
			}
		}
	}
	LevelRoomData->CacheDoorTransforms(ExcludeActor);
}

#endif

void ABaseDoorSlot::OnConstruction(const FTransform& Transform)
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		return;
	}
	Super::OnConstruction(Transform);

#if WITH_EDITOR

	NotifyRoomData(nullptr);
	
#endif
}

// Called every frame
void ABaseDoorSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

