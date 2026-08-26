using UnrealBuildTool;

public class GameplayCore : ModuleRules
{
	public GameplayCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
