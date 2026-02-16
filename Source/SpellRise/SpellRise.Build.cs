using UnrealBuildTool;

public class SpellRise : ModuleRules
{
	public SpellRise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = true;

		// Mantém consistente com UE 5.7
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"GameplayTags",
				"GameplayAbilities",
				"GameplayTasks",

				"AudioMixerCore",
				"AudioPlatformConfiguration"
			}

		);
		
	}
}