using UnrealBuildTool;

public class SpellRise : ModuleRules
{
	public SpellRise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = true;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ApplicationCore",
				"PhysicsCore",
				"InputCore",
				"EnhancedInput",
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				"UMG",
				"Niagara",
				"ModularGameplay"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// UI / Widgets
				"Slate",
				"SlateCore",

				// Rendering / FX support
				"RenderCore",
				"RHI",

				// Networking / multiplayer
				"NetCore",

				// General project/runtime support
				"Projects",
				"DeveloperSettings",

				// Audio
				"AudioMixer"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}