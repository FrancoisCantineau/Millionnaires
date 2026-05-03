// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Damages/DamageStatic.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"

void UDamageStatic::ApplyImpactDamageToActor(const FDamageData& DamageData, AActor* TargetActor, FVector ImpactPoint)
{

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamageData.SourceActor);
	
	if (SourceASC && DamageData.ImpactCueTag.IsValid())
	{
		FGameplayCueParameters Params;
		Params.Location = ImpactPoint;
		Params.RawMagnitude = DamageData.Radius;

		SourceASC->ExecuteGameplayCue(
			DamageData.ImpactCueTag,
			Params
		);
	}
	
	if (!DamageData.SourceActor || !TargetActor)
		return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!TargetASC)
		return;
	
	for (auto& EffectClass : DamageData.AdditionalEffects)
	{
		if (!EffectClass) continue;

		FGameplayEffectContextHandle EffectContext =
			TargetASC->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle =
			TargetASC->MakeOutgoingSpec(EffectClass, 1.f, EffectContext);

		if (SpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	if (DamageData.DamageSpec.IsValid() && DamageData.DamageSpec.Data.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageData.DamageSpec.Data.Get());
	}
}

void UDamageStatic::ApplyRadialDamage(const FDamageData& DamageData, FVector OriginPoint)
{
	if (!DamageData.SourceActor || DamageData.Radius <= 0.f)
		return;

	UWorld* World = DamageData.SourceActor->GetWorld();
	if (!World)
		return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamageData.SourceActor);
	
	TArray<FOverlapResult> Overlaps;

	FCollisionShape Sphere=FCollisionShape::MakeSphere(DamageData.Radius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(DamageData.SourceActor);

	World->OverlapMultiByChannel(
		Overlaps,
		OriginPoint,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);
	
	if (SourceASC && DamageData.ExplosionCueTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = OriginPoint;
		CueParams.RawMagnitude = DamageData.Radius;

		SourceASC->ExecuteGameplayCue(
			DamageData.ExplosionCueTag,
			CueParams
		);
	}
	
	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor) continue;
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (TargetASC && DamageData.DamageSpec.IsValid())
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageData.DamageSpec.Data.Get());
	}
}
