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
	 *  is entirely up to your project-side wiring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bBroadcastIfPlayerFar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FDialogueNode> Nodes;

	/** Finds a node by ID. Returns nullptr if not found. O(1) after the first call (builds an
	 *  internal index cache lazily); authoring stays a plain array for comfortable editing. */
	const FDialogueNode* FindNode(FName NodeID) const;

#if WITH_EDITOR
	/** Checks every choice's and every linear NextNodeID resolves to an existing node (or
	 *  NAME_None, which validly ends the dialogue). Catches typos that would otherwise only
	 *  surface as the dialogue silently stopping at runtime. Callable from the Details panel. */
	UFUNCTION(CallInEditor, Category = "Dialogue|Validation")
	bool ValidateNodeLinks(TArray<FString>& OutErrors) const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	/** Lazily-built NodeID -> index cache for FindNode. Invalidated on any editor edit
	 *  (PostEditChangeProperty) and rebuilt if its size doesn't match Nodes (covers the
	 *  runtime/cooked-game case, where the asset never changes after load). */
	mutable TMap<FName, int32> NodeIndexCache;
	void RebuildNodeIndexCacheIfNeeded() const;
};
