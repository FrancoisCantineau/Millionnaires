#include "IconProviderLocator.h"
#include "IconProviderRegistry.h"

IIconProvider* FIconProviderLocator::Get(UObject* WorldContext)
{
	return FIconProviderRegistry::Get();
} 