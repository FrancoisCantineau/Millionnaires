// DialogueDataAsset.cpp
#include "DialogueDataAsset.h"

const FDialogueNode* UDialogueDataAsset::FindNode(FName NodeID) const
{
	RebuildNodeIndexCacheIfNeeded();

	if (const int32* Index = NodeIndexCache.Find(NodeID))
	{
		return &Nodes[*Index];
	}
	return nullptr;
}

void UDialogueDataAsset::RebuildNodeIndexCacheIfNeeded() const
{
	if (NodeIndexCache.Num() == Nodes.Num())
	{
		return;
	}

	NodeIndexCache.Reset();
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		NodeIndexCache.Add(Nodes[i].NodeID, i);
	}
}

#if WITH_EDITOR
bool UDialogueDataAsset::ValidateNodeLinks(TArray<FString>& OutErrors) const
{
	OutErrors.Reset();

	TSet<FName> NodeIDs;
	for (const FDialogueNode& Node : Nodes)
	{
		if (NodeIDs.Contains(Node.NodeID))
		{
			OutErrors.Add(FString::Printf(TEXT("Duplicate NodeID: %s"), *Node.NodeID.ToString()));
		}
		NodeIDs.Add(Node.NodeID);
	}

	if (StartNodeID != NAME_None && !NodeIDs.Contains(StartNodeID))
	{
		OutErrors.Add(FString::Printf(TEXT("StartNodeID '%s' does not exist."), *StartNodeID.ToString()));
	}

	for (const FDialogueNode& Node : Nodes)
	{
		if (Node.Choices.Num() > 0)
		{
			for (const FDialogueChoice& Choice : Node.Choices)
			{
				if (Choice.NextNodeID != NAME_None && !NodeIDs.Contains(Choice.NextNodeID))
				{
					OutErrors.Add(FString::Printf(TEXT("Node '%s': choice '%s' points to missing NodeID '%s'."),
						*Node.NodeID.ToString(), *Choice.ChoiceText.ToString(), *Choice.NextNodeID.ToString()));
				}
			}
		}
		else if (Node.NextNodeID != NAME_None && !NodeIDs.Contains(Node.NextNodeID))
		{
			OutErrors.Add(FString::Printf(TEXT("Node '%s': NextNodeID points to missing NodeID '%s'."),
				*Node.NodeID.ToString(), *Node.NextNodeID.ToString()));
		}
	}

	const bool bValid = OutErrors.Num() == 0;
	if (bValid)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] ValidateNodeLinks: OK, no issues found."), *GetName());
	}
	else
	{
		for (const FString& Error : OutErrors)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] ValidateNodeLinks: %s"), *GetName(), *Error);
		}
	}

	return bValid;
}

void UDialogueDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	NodeIndexCache.Reset();
}
#endif
