// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Components/Resources/WeaponResourceComponentBase.h"

#include "Components/WidgetComponent.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/Widget/AmmoWidgetBase.h"

// Sets default values for this component's properties
UWeaponResourceComponentBase::UWeaponResourceComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

}


bool UWeaponResourceComponentBase::CanReload()
{
	return false;
}


void UWeaponResourceComponentBase::ChangesApplied(int Current, int Max)
{
	OnResourceChanged.Broadcast(Current, Max);
}

// Called when the game starts
void UWeaponResourceComponentBase::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerWeapon = Cast<AWeaponBase>(GetOwner());
	
	if (OwnerWeapon)
        {
            
            TArray<UWidgetComponent*> WidgetComponents;
            OwnerWeapon->GetComponents<UWidgetComponent>(WidgetComponents);
            
            for (UWidgetComponent* WidgetComp : WidgetComponents)
            {
                if (WidgetComp && WidgetComp->GetWidget())
                {
                    LinkedWidget = Cast<UAmmoWidgetBase>(WidgetComp->GetWidget());
                    if (LinkedWidget)
                    {
                  
                        LinkedWidget->BindResources(this);
                        break;
                    }
                }
            }
        }
}


// Called every frame
void UWeaponResourceComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UWeaponResourceComponentBase::CanConsume() const
{
	return false;
}

bool UWeaponResourceComponentBase::Consume()
{
	return false;
}

void UWeaponResourceComponentBase::Reload()
{
}