using UnrealBuildTool;

public class CodexBlueprintBridgeTests : ModuleRules
{
    public CodexBlueprintBridgeTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CodexBlueprintBridge",
            "Core",
            "CoreUObject",
            "Engine",
            "Json"
        });

        PrivateIncludePaths.Add(
            System.IO.Path.Combine(
                PluginDirectory,
                "Source",
                "CodexBlueprintBridge",
                "Private"));
    }
}
