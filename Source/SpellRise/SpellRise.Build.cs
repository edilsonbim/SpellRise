using UnrealBuildTool;

public class SpellRise : ModuleRules
{
	public SpellRise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoPCHs; // CRÍTICO: Mudar de UseExplicitOrSharedPCHs para NoPCHs
		bUseUnity = false; // Desabilitar Unity Build para LiveCoding
		
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayAbilities",
			"GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] {
			"UnrealEd",
			"PropertyEditor",
			"AudioMixer",
			"EditorStyle"
		});
	}
}