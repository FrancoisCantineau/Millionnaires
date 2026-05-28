#include "IconProviderRegistry.h"

IIconProvider* FIconProviderRegistry::Instance = nullptr;

void FIconProviderRegistry::Register(IIconProvider* Provider)
{
	Instance = Provider;
}

IIconProvider* FIconProviderRegistry::Get()
{
	return Instance;
}