// DialogueDataAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueSystemTypes.h"
#include "DialogueDataAsset.generated.h"

/**
 * A complete dialogue: a set of nodes plus which one to start at. Author these in the editor —
 * no code needed per dialogue. A "linear" dialogue is just one where every node has empty Choices.
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEM_API UDialogueDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName StartNodeID;

	/** If true, this dialogue is important enough that the project should make sure the player
	 *  can hear it even if they're too far from the speaking Actor for its normal voice to carry
	 *  (e.g. relay it over nearby speakers instead/as well). Pure data — how that's actually done
	 *  (which speakers, what "too far" means) is entirely up to your project-side wiring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bBroadcastIfPlayerFar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueNode> Nodes;

	/** Finds a node by ID. Returns nullptr if not found. */
	const FDialogueNode* FindNode(FName NodeID) const;
};
