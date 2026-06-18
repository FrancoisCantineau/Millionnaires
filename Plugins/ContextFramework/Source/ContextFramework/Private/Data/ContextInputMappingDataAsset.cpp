// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ContextInputMappingDataAsset.h"

#include "InputMappingContext.h"
#include "EnhancedActionKeyMapping.h"

#if WITH_EDITOR
void UContextInputMappingDataAsset::RefreshMappings()
{
	if (!MappingContext)
	{
		return;
	}

	TSet<const UInputAction*> ExistingActions;

	for (const FInputTagMapping& Entry : Mappings)
	{
		if (Entry.Action)
		{
			ExistingActions.Add(Entry.Action.Get());
		}
	}

	TSet<const UInputAction*> AddedActions;

	for (const FEnhancedActionKeyMapping& Mapping :
		MappingContext->GetMappings())
	{
		if (!Mapping.Action)
		{
			continue;
		}

		const UInputAction* Action =
			Mapping.Action.Get();
		
		if (AddedActions.Contains(Action))
		{
			continue;
		}

		AddedActions.Add(Action);

		if (ExistingActions.Contains(Action))
		{
			continue;
		}

		FInputTagMapping NewEntry;
		NewEntry.Action = Action;

		Mappings.Add(NewEntry);
	}

	MarkPackageDirty();
}

void UContextInputMappingDataAsset::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(
		PropertyChangedEvent);

	const FName PropertyName =
		PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (PropertyName ==
		GET_MEMBER_NAME_CHECKED(
			UContextInputMappingDataAsset,
			MappingContext))
	{
		RefreshMappings();
	}
}
void UContextInputMappingDataAsset::CleanUnusedMappings()
{
	if (!MappingContext)
	{
		return;
	}

	TSet<const UInputAction*> ValidActions;

	for (const FEnhancedActionKeyMapping& Mapping :
		MappingContext->GetMappings())
	{
		if (Mapping.Action)
		{
			ValidActions.Add(
				Mapping.Action.Get());
		}
	}

	Mappings.RemoveAll(
		[&ValidActions](const FInputTagMapping& Entry)
		{
			return !Entry.Action ||
				!ValidActions.Contains(
					Entry.Action.Get());
		});

	MarkPackageDirty();
}
#endif
