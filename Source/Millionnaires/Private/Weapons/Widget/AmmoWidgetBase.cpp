// Fill out your copyright notice in the Description page of Project Settings.

/*
 * Millionaire Project, 2026
 * Created by:  "Francki"
 * Last Updated by: "Francki"
 * Class: "AmmoWidgetBase" - Source
 * Notes: Basic Widget to display the amount of ressources left. Used for the world space resources informations.
 */

#include "Weapons/Widget/AmmoWidgetBase.h"

void UAmmoWidgetBase::UpdateResources_Implementation(int m_CurrentAmmos, int m_RemainingAmmos)
{
	
}

void UAmmoWidgetBase::BindResources(UWeaponResourceComponentBase* ResourceComp)
{
	if (!ResourceComp) return;
	ResourceComp->OnResourceChanged.AddDynamic(this, &UAmmoWidgetBase::UpdateResources);
}
