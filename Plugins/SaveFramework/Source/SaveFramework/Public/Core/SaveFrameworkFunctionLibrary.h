#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveFrameworkFunctionLibrary.generated.h"

/**
 * Small Blueprint-facing utilities. Mainly useful for manual testing and
 * quick debugging — a designer copies a GUID as text (e.g. from the
 * SaveGuidComponent details panel) and needs to turn it back into an FGuid
 * to pass to USaveFrameworkWorldState.
 */
UCLASS()
class SAVEFRAMEWORK_API USaveFrameworkFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Parses a GUID string (e.g. "4F2A1B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C") back into an FGuid. bSuccess is false if the string isn't a valid GUID. */
	UFUNCTION(BlueprintCallable, Category = "SaveFramework", meta = (ExpandBoolAsExecs = "bSuccess"))
	static FGuid ParseSaveId(const FString& GuidString, bool& bSuccess);

	/** Human-readable string form of a GUID — handy for copy-pasting into a test, or logging. */
	UFUNCTION(BlueprintPure, Category = "SaveFramework")
	static FString SaveIdToString(const FGuid& SaveId);
};
