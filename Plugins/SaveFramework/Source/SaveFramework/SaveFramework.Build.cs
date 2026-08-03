using UnrealBuildTool;

public class SaveFramework : ModuleRules
{
	public SaveFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"StructUtils" // gives us FInstancedStruct, used to store any USTRUCT generically
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// IMPORTANT: never add a dependency on a game module or another gameplay
		// plugin (Inventory, AI, EventSystem, etc.) here. If a future feature
		// needs one, that feature belongs in the game project, not in this plugin.
	}
}
