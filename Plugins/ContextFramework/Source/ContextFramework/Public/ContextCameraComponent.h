#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "ContextCameraComponent.generated.h"

class UContextCameraSetupDataAsset;
struct FActiveContext;
class APlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CONTEXTFRAMEWORK_API UContextCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UContextCameraComponent();

	bool MustReceiveLookInput(){return InputReceiver;};
	
	void ConsumeLookInput(FVector2D Value);

protected:

	UPROPERTY()
	FRotator BaseWorldRotation;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> CachedFocusTarget;
	
	bool InputReceiver;
	
	virtual void BeginPlay() override;

	void HandleContextAdded(const FActiveContext& Context);
	void HandleContextRemoved(const FActiveContext& Context);

	APlayerController* GetPlayerController() const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void ApplyCameraConstraints(APlayerController* PC, const UContextCameraSetupDataAsset* Data);
	void ResetCameraConstraints(APlayerController* PC);

private:

	const UContextCameraSetupDataAsset* CurrentCameraData = nullptr;

	UPROPERTY()
	bool bContextActive = false;

	UPROPERTY()
	FRotator BaseContextRotation;

	UPROPERTY()
	float BaseContextYaw;
	
	UPROPERTY()
	FVector FocusPoint;

	UPROPERTY()
	bool bHasFocus = false;

	void SetFocusPoint(FVector Point)
	{
		FocusPoint = Point;
		bHasFocus = true;
	}
	
	void UpdateFocus(APlayerController* PC);
	void ClearFocus()
	{
		bHasFocus = false;
	}

	UPROPERTY()
	UCameraComponent* Camera = nullptr;

	bool bCameraBlending = false;

	float BlendDuration = 0.5f;

	FQuat HeadToCameraOffset;
	bool bReturningToFPS = false;
	float BlendElapsed = 0.f;

	UPROPERTY()
	FRotator LookOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	float InputSensitivity = 1.0f;

	FVector BlendStartLocation;
	FRotator BlendStartRotation;
	
	FRotator BaseCameraRotation;
	
};