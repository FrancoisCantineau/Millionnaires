#include "Core/SaveFrameworkFunctionLibrary.h"

FGuid USaveFrameworkFunctionLibrary::ParseSaveId(const FString& GuidString, bool& bSuccess)
{
	FGuid Result;
	bSuccess = FGuid::Parse(GuidString, Result);
	return Result;
}

FString USaveFrameworkFunctionLibrary::SaveIdToString(const FGuid& SaveId)
{
	return SaveId.ToString();
}
