using UnrealBuildTool;

public class SpellRise : ModuleRules
{
	public SpellRise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;
        
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
        
		// NÃO ADICIONE ISTO:
		// PrivateIncludePaths.Add("SpellRise/Combat/Melee/Components");
		// PublicAdditionalLibraries.Add(...);
		// NÃO ADICIONE ARQUIVOS .cpp MANUALMENTE!
	}
}