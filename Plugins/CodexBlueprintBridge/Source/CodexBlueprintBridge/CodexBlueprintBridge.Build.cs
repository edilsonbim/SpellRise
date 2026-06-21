using UnrealBuildTool;

public class CodexBlueprintBridge : ModuleRules
{
    public CodexBlueprintBridge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "AssetRegistry",
            "BlueprintGraph",
            "ContentBrowser",
            "EditorScriptingUtilities",
            "Json",
            "JsonUtilities",
            "Kismet",
            "KismetCompiler",
            "Projects",
            "Slate",
            "SlateCore",
            "ToolMenus",
            "UMG",
            "UMGEditor",
            "UnrealEd"
        });
    }
}
