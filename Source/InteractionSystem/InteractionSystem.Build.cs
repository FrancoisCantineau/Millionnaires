using UnrealBuildTool;

public class InteractionSystem : ModuleRules
{
	public InteractionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"IrisCore",
				"InputCore",
				"UMG",
				"Slate",
				"SlateCore",
				"GameInterfaces",
				"ItemSystem",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"InventorySystem",
			}
		);
	}
}