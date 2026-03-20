// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Components/ArrowComponent.h"
#include "BaseDoorSlot.generated.h"

class ABaseRoomData;

UCLASS()
class MILLIONNAIRES_API ABaseDoorSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoorSlot();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Door")
	FTransform GetDoorTransform() const {return ArrowComponent->GetComponentTransform();}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorBaseArrow")
	bool bIsUsed = false;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Debug")
	FString DebugName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FGameplayTag ZoneType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FGameplayTag DoorType;
	
	UPROPERTY(BlueprintReadWrite, Category = "Door")
	ABaseRoomData* ParentRoom = nullptr;

#if WITH_EDITOR
	virtual void Destroyed() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void NotifyRoomData(AActor* ExcludeActor);
#endif
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class ABaseRoomData* LevelRoomData = nullptr;





public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
