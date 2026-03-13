// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBaseArrow.generated.h"

UCLASS()
class MILLIONNAIRES_API ADoorBaseArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorBaseArrow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorBaseArrow")
	bool bIsUsed = false;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
