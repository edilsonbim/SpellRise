using UnrealBuildTool;

public class SpellRiseEditorTarget : TargetRules
{
	public SpellRiseEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_7;

		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("SpellRise");
	}
}
