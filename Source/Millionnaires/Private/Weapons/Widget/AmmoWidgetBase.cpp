// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Widget/AmmoWidgetBase.h"

void UAmmoWidgetBase::UpdateResources_Implementation(int m_CurrentAmmos, int m_RemainingAmmos)
{
	
}

void UAmmoWidgetBase::BindResources(UWeaponResourceComponentBase* ResourceComp)
{
	if (!ResourceComp) return;
	ResourceComp->OnResourceChanged.AddDynamic(this, &UAmmoWidgetBase::UpdateResources);
}
