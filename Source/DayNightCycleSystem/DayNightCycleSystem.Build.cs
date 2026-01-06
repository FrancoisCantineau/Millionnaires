using UnrealBuildTool;

public class DayNightCycleSystem : ModuleRules
{
    public DayNightCycleSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "CoreUObject", 
                "Engine",
                "InputCore",
                "UMG",
                "Slate",
                "SlateCore" 
            }
        );
    }
}