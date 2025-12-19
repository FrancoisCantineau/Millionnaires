using UnrealBuildTool;

public class GameInterfaces : ModuleRules
{
    public GameInterfaces(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                // Never add dependencies here please
            }
        );
    }
}