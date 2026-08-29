// MetroStation.cpp
#include "MetroStation.h"
#include "MetroLineComponent.h"

AMetroStation::AMetroStation()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}
	
UMetroLineComponent* AMetroStation::GetLineComponent() const
{
	if (AActor* Parent = GetAttachParentActor())
	{
		return Parent->FindComponentByClass<UMetroLineComponent>();
	}
	return nullptr;
}