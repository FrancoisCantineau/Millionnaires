#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "Components/ShipScreenWidgetComponent.h"
#include "GameFramework/Actor.h"
#include "ShipScreenActor.generated.h"

class UWidgetComponent;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
class UUserWidget;

UCLASS()
class MILLIONNAIRES_API AShipScreenActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	AShipScreenActor();
	
	UFUNCTION(BlueprintCallable, Category = "Screen")
	void SetScreenWidget(UUserWidget* InWidget);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Screen")
	UStaticMeshComponent* ScreenMesh;

	UPROPERTY(VisibleAnywhere, Category = "Screen")
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Screen")
	TSubclassOf<UUserWidget> DefaultWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Screen")
	USceneComponent* CameraViewPoint;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionDisplayName_Implementation() const override;

	void StartViewing(APlayerController* PC);

	FVector TargetLocation;
	FRotator TargetRotation;

	ACameraActor* ScreenCamera = nullptr;
};
