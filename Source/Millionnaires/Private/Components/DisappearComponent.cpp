// DisappearComponent.cpp
#include "Components/DisappearComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

UDisappearComponent::UDisappearComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
 
void UDisappearComponent::SetDisappearState(EDisappearState NewState)
{
	switch (NewState)
	{
	case EDisappearState::Disappeared:
		Disappear();
		break;
	case EDisappearState::Appeared:
		Appear();
		break;
	}
}
 
void UDisappearComponent::Disappear()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
 
	if (DisappearSound)
	{
		UGameplayStatics::PlaySoundAtLocation(Owner, DisappearSound, Owner->GetActorLocation());
	}
 
	Owner->SetActorHiddenInGame(true);
	Owner->SetActorEnableCollision(false);
	bIsDisappeared = true;
}
 
void UDisappearComponent::Appear()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
 
	if (AppearSound)
	{
		UGameplayStatics::PlaySoundAtLocation(Owner, AppearSound, Owner->GetActorLocation());
	}
 
	Owner->SetActorHiddenInGame(false);
	Owner->SetActorEnableCollision(true);
	bIsDisappeared = false;
}