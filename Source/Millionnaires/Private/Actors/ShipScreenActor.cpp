// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ShipScreenActor.h"

#include "ContextComponent.h"
#include "Data/ContextStructData.h"

#include "MillionnairesPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShipScreenActor::AShipScreenActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	RootComponent = ScreenMesh;

	CameraViewPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CameraViewPoint"));
	CameraViewPoint->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(512.f, 256.f));
}

void AShipScreenActor::BeginPlay()
{
	Super::BeginPlay();

	if (!DefaultWidgetClass) return;
	
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, [this]()
	{
		WidgetComponent->SetWidgetClass(DefaultWidgetClass);
		WidgetComponent->InitWidget();
	}, 2.f, false);

	FActorSpawnParameters Params;
	ScreenCamera = GetWorld()->SpawnActor<ACameraActor>(
		ACameraActor::StaticClass(),
		CameraViewPoint->GetComponentLocation(),
		CameraViewPoint->GetComponentRotation(),
		Params
	);

	ScreenCamera->GetCameraComponent()->bConstrainAspectRatio = false;	
}

void AShipScreenActor::Interact_Implementation(AActor* Interactor)
{
	IInteractionInterface::Interact_Implementation(Interactor);

	UContextComponent* Contexts =
		Interactor->FindComponentByClass<UContextComponent>();

	FActiveContext Context;

	Context.Definition = ContextDataAsset;
	Context.ViewTarget = ScreenCamera;

	Contexts->AddContext(Context);
	/*
	APlayerController* PC = Cast<APlayerController>(
	   Cast<APawn>(Interactor)->GetController()
   );
	if (!PC) return;

	StartViewing(PC);*/
}

FText AShipScreenActor::GetInteractionDisplayName_Implementation() const
{
	return IInteractionInterface::GetInteractionDisplayName_Implementation();
}

void AShipScreenActor::StartViewing(APlayerController* PC)
{
	if (AMillionnairesPlayerController* PCs =
			Cast<AMillionnairesPlayerController>(PC))
	{
		PCs->SetPlayerMode(EPlayerMode::Inspect, ScreenCamera);
	}
}


void AShipScreenActor::SetScreenWidget(UUserWidget* InWidget)
{
	if (!InWidget) return;
	WidgetComponent->SetWidget(InWidget);
}
