// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MillionnaireSaveManager.h"
#include "Kismet/GameplayStatics.h"
#include "System/MillionnaireSaveGame.h"

void UMillionnaireSaveManager::RegisterSystem(TScriptInterface<ISaveDataInterface> System)
{
	RegisteredSystems.AddUnique(System);
}

void UMillionnaireSaveManager::SaveAll()
{
	UMillionnaireSaveGame* SaveObject = Cast<UMillionnaireSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMillionnaireSaveGame::StaticClass())
	);
	for (auto& System : RegisteredSystems)
		System->OnSave(SaveObject);
	UGameplayStatics::SaveGameToSlot(SaveObject, "ShipSave_0", 0);
}

void UMillionnaireSaveManager::LoadAll()
{
	UMillionnaireSaveGame* SaveObject = Cast<UMillionnaireSaveGame>(
	   UGameplayStatics::LoadGameFromSlot("ShipSave_0", 0)
   );
	if (!SaveObject) return;
	for (auto& System : RegisteredSystems)
		System->OnLoad(SaveObject);
}
