// BTTask_UseAbility.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_UseAbility.generated.h"

/**
 * EXPLORATORY — not yet wired into any real branch.
 *
 * Sends a Gameplay Event to the controlled Pawn's Ability System Component, optionally carrying
 * a target Actor and/or a world Location. Succeeds only if the event actually triggered at least
 * one Ability (via ASC->HandleGameplayEvent's return count) — not just because the event was
 * sent, since nothing may have been listening for it.
 *
 * Covers Disappear (neither), Teleport (Location), Attack (Actor), AttackAtPoint (both) — a
 * small, bounded vocabulary, not a payload for every imaginable case. If a future Ability needs
 * something this can't express (multiple points, a curve, a direction), that's a signal to build
 * a small dedicated TargetData for THAT ability, not to keep adding optional fields here.
 *
 * Does not yet wait for the Ability to end — it only reports whether one started. Whether some
 * branches should block until EndAbility() is a separate problem to solve once this base works.
 *
 * TargetActorKey/LocationKey are simply left unassigned in the editor when not needed — no
 * separate "UseTargetActor"/"UseLocation" toggle is needed, an unassigned FBlackboardKeySelector
 * already means "skip this part of the payload".
 */
UCLASS()
class MILLIONNAIRES_API UBTTask_UseAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_UseAbility();

	/** Gameplay Event tag that triggers the receiving Ability — e.g. Ability.Disappear, Ability.Teleport. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTag EventTag;

	/** Leave unassigned if this Ability doesn't need a target Actor. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Leave unassigned if this Ability doesn't need a location. */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LocationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif
};