// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/CharacterStateComponent.h"
#include "ContextComponent.h"
#include "System/Tags/MillionnaireGameplayTags_States.h"

UCharacterStateComponent::UCharacterStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterStateComponent::ResolveState()
{
	if (ActiveStates.HasTag(TAG_State_Ladder))
	{
		CurrentState = ELocomotionState::Ladder;
		return;
	}
	
	if (ActiveStates.HasTag(TAG_State_Default))
	{
		CurrentState = ELocomotionState::Default;
		return;
	}

	if (ActiveStates.HasTag(TAG_State_Crawl))
	{
		CurrentState = ELocomotionState::Crawl;
		return;
	}

	if (ActiveStates.HasTag(TAG_State_Crouched))
	{
		CurrentState = ELocomotionState::Crouch;
		return;
	}

	CurrentState = ELocomotionState::Default;
}

void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ContextComponent = GetOwner()->FindComponentByClass<UContextComponent>();

	if (!ContextComponent)
		return;

	ContextComponent->OnContextTagChanged.AddUObject(
	this,
	&UCharacterStateComponent::HandleTagChanged);
	
}

bool UCharacterStateComponent::HasState(FGameplayTag Tag) const
{
	return ActiveStates.HasTagExact(Tag);
}

void UCharacterStateComponent::AddState(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	if (ActiveStates.HasTagExact(Tag))
		return;

	ActiveStates.AddTag(Tag);

	ResolveState();
	
	OnStateTagChanged.Broadcast(Tag, true);
}

void UCharacterStateComponent::RemoveState(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	if (!ActiveStates.HasTagExact(Tag))
		return;

	ActiveStates.RemoveTag(Tag);

	ResolveState();
	
	OnStateTagChanged.Broadcast(Tag, false);
}

void UCharacterStateComponent::HandleTagChanged(const FContextTagChange& Change)
{
	if (Change.bAdded)
	{
		AddState(Change.Tag);
	}
	else
	{
		RemoveState(Change.Tag);
	}
}
