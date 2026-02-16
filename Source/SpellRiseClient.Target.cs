using UnrealBuildTool;

public class SpellRiseClientTarget : TargetRules
{
	public SpellRiseClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_7;

		CppStandard = CppStandardVersion.Cpp20;

		// Opcional: monolithic no client
		LinkType = TargetLinkType.Monolithic;

		ExtraModuleNames.Add("SpellRise");
	}
}
