// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Events/IncidentManager.h"

FGuid UIncidentManager::TriggerIncident(FShipIncident Incident)
{
	Incident.IncidentID = FGuid::NewGuid();
	Incident.bIsResolved = false;
	
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		Incident.StartTime = GetGameInstance()->GetWorld()->GetTimeSeconds();
	}
	
	ActiveIncidents.Add(Incident);
	
	if (Incident.bHasDuration && Incident.Duration > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{
			FTimerHandle Timer;
			FGuid IncidentID = Incident.IncidentID;
 
			World->GetTimerManager().SetTimer(
				Timer,
				[this, IncidentID]()
				{
					AutoResolveIncident(IncidentID);
				},
				Incident.Duration,
				false
			);
 
			IncidentTimers.Add(Incident.IncidentID, Timer);
		}
	}
 
	// Broadcast à tous les abonnés
	OnIncidentTriggered.Broadcast(Incident);
 
	return Incident.IncidentID;
}

void UIncidentManager::ResolveIncident(FGuid IncidentID)
{
	for (FShipIncident& Incident : ActiveIncidents)
	{
		if (Incident.IncidentID != IncidentID) continue;
		if (Incident.bIsResolved) return;
 
		Incident.bIsResolved = true;
 
		// Annule le timer si existant
		if (FTimerHandle* Timer = IncidentTimers.Find(IncidentID))
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(*Timer);
			}
			IncidentTimers.Remove(IncidentID);
		}
 
		OnIncidentResolved.Broadcast(Incident);
 
		// Retire de la liste
		ActiveIncidents.RemoveAll([IncidentID](const FShipIncident& I)
		{
			return I.IncidentID == IncidentID;
		});
 
		return;
	}
}

void UIncidentManager::ResolveAllIncidentsInZone(FGameplayTag Zone)
{
	TArray<FGuid> ToResolve;
 
	for (const FShipIncident& Incident : ActiveIncidents)
	{
		if (Incident.AffectedZone == Zone)
		{
			ToResolve.Add(Incident.IncidentID);
		}
	}
 
	for (const FGuid& ID : ToResolve)
	{
		ResolveIncident(ID);
	}
}

void UIncidentManager::ResolveAllIncidents()
{
	TArray<FGuid> ToResolve;
 
	for (const FShipIncident& Incident : ActiveIncidents)
	{
		ToResolve.Add(Incident.IncidentID);
	}
 
	for (const FGuid& ID : ToResolve)
	{
		ResolveIncident(ID);
	}
}

TArray<FShipIncident> UIncidentManager::GetActiveIncidents() const
{
	return ActiveIncidents;
}

TArray<FShipIncident> UIncidentManager::GetActiveIncidentsInZone(FGameplayTag Zone) const
{
	TArray<FShipIncident> Result;
 
	for (const FShipIncident& Incident : ActiveIncidents)
	{
		if (Incident.AffectedZone == Zone)
		{
			Result.Add(Incident);
		}
	}
 
	return Result;
}

TArray<FShipIncident> UIncidentManager::GetActiveIncidentsOnFloor(int32 FloorIndex) const
{
	TArray<FShipIncident> Result;
 
	for (const FShipIncident& Incident : ActiveIncidents)
	{
		if (Incident.AffectedFloor == FloorIndex || Incident.AffectedFloor == -1)
		{
			Result.Add(Incident);
		}
	}
 
	return Result;	
}

bool UIncidentManager::HasActiveIncident(FGameplayTag IncidentTag, FGameplayTag Zone) const
{
	for (const FShipIncident& Incident : ActiveIncidents)
	{
		if (Incident.IncidentTag == IncidentTag && Incident.AffectedZone == Zone)
		{
			return true;
		}
	}
 
	return false;
}

void UIncidentManager::AutoResolveIncident(FGuid IncidentID)
{
	ResolveIncident(IncidentID);
}
