using UnrealBuildTool;

public class TraversalSystem : ModuleRules
{
	public TraversalSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"AnimGraphRuntime",
				"GameInterfaces",
				"GameplayTags"
			}
		);
	}
}