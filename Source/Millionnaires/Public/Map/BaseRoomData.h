// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BaseDoorSlot.h"
#include "Components/BoxComponent.h"
#include "BaseRoomData.generated.h"


enum class ERoomType : uint8;


USTRUCT(BlueprintType)
struct FDoorSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ABaseDoorSlot* Door = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FTransform Transform;
};

UCLASS()
class MILLIONNAIRES_API ABaseRoomData : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseRoomData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorBaseArrow")
	TArray<USceneComponent*> ConnexionArray;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Debug")
	FString DebugName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	FGameplayTag ZoneType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	ERoomType RoomType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Room")
	TSubclassOf<AActor> MiniVersion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	TArray<FDoorSlotData> DoorSlots;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Room")
	TArray<FDoorSlotData> GetFreeDoors() ;

	UFUNCTION(BlueprintCallable, Category = "Room")
	FDoorSlotData GetRandomFreeDoor();
	
	UFUNCTION(BlueprintCallable, Category = "Room")
	void MarkDoorAsUsed(ABaseDoorSlot* Door);

	UFUNCTION(BlueprintCallable, Category = "Room")
	void CacheDoorTransforms(AActor* Excluded);

protected:

	UFUNCTION(CallInEditor, Category="Room")
	void RefreshDoors();
	
};

