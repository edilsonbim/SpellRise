using UnrealBuildTool;

public class SpellRiseClientTarget : TargetRules
{
	public SpellRiseClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		ExtraModuleNames.Add("SpellRise");
	}
}