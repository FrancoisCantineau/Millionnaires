// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IncidentManager.generated.h"


USTRUCT(BlueprintType)
struct FShipIncident
{
	GENERATED_BODY()
 
	// Identifiant unique de l'incident
	UPROPERTY(BlueprintReadOnly)
	FGuid IncidentID;
 
	// Type d'incident : Incident.Fire, Incident.Power, Incident.Hull...
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag IncidentTag;
 
	// Zone affectée — vide = global
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag AffectedZone;
 
	// Étage affecté — -1 = tout le vaisseau
	UPROPERTY(BlueprintReadWrite)
	int32 AffectedFloor = -1;
 
	// L'incident a-t-il une durée automatique ?
	UPROPERTY(BlueprintReadWrite)
	bool bHasDuration = false;
 
	// Durée en secondes (si bHasDuration)
	UPROPERTY(BlueprintReadWrite)
	float Duration = 0.0f;
 
	// L'incident est-il résolu ?
	UPROPERTY(BlueprintReadOnly)
	bool bIsResolved = false;
 
	// Temps de début (via GetGameTimeSinceCreation)
	UPROPERTY(BlueprintReadOnly)
	float StartTime = 0.0f;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer AffectedSystems;
};

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncidentTriggered, FShipIncident, Incident);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncidentResolved, FShipIncident, Incident);

UCLASS()
class MILLIONNAIRES_API UIncidentManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public :
	
	UPROPERTY(BlueprintAssignable, Category = "Incidents")
	FOnIncidentTriggered OnIncidentTriggered;
	
	UPROPERTY(BlueprintAssignable, Category = "Incidents")
	FOnIncidentResolved OnIncidentResolved;

	UFUNCTION(BlueprintCallable, Category = "Incidents")
	FGuid TriggerIncident(FShipIncident Incident);
 
	// Résout un incident par son ID
	UFUNCTION(BlueprintCallable, Category = "Incidents")
	void ResolveIncident(FGuid IncidentID);
 
	// Résout tous les incidents d'une zone
	UFUNCTION(BlueprintCallable, Category = "Incidents")
	void ResolveAllIncidentsInZone(FGameplayTag Zone);
 
	// Résout tous les incidents actifs
	UFUNCTION(BlueprintCallable, Category = "Incidents")
	void ResolveAllIncidents();
 
	// Retourne tous les incidents actifs
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Incidents")
	TArray<FShipIncident> GetActiveIncidents() const;
 
	// Retourne les incidents actifs d'une zone
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Incidents")
	TArray<FShipIncident> GetActiveIncidentsInZone(FGameplayTag Zone) const;
 
	// Retourne les incidents actifs d'un étage
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Incidents")
	TArray<FShipIncident> GetActiveIncidentsOnFloor(int32 FloorIndex) const;
 
	// Vérifie si une zone a un incident actif d'un type donné
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Incidents")
	bool HasActiveIncident(FGameplayTag IncidentTag, FGameplayTag Zone) const;

private:
	
	TArray<FShipIncident> ActiveIncidents;
	
	TMap<FGuid, FTimerHandle> IncidentTimers;
	
	void AutoResolveIncident(FGuid IncidentID);
};
