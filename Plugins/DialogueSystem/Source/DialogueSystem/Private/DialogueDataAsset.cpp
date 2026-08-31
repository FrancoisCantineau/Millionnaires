// DialogueDataAsset.cpp
#include "DialogueDataAsset.h"

const FDialogueNode* UDialogueDataAsset::FindNode(FName NodeID) const
{
	return Nodes.FindByPredicate([NodeID](const FDialogueNode& Node)
	{
		return Node.NodeID == NodeID;
	});
}
