// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAction_CameraShake.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameplayEventContext.h"
 
void UGameplayAction_CameraShake::Execute_Implementation(const FEventContext& Context)
{
	if (!ShakeClass)
	{
		return;
	}
 
	UWorld* World = Context.Sender.IsValid() ? Context.Sender->GetWorld() : (GEngine ? GEngine->GetCurrentPlayWorld() : nullptr);
	if (!World)
	{
		return;
	}
 
	// No local player = nobody around to feel the shake. Silently skip.
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC || !PC->PlayerCameraManager)
	{
		return;
	}
 
	PC->PlayerCameraManager->StartCameraShake(ShakeClass, Scale);
}
