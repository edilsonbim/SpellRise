using UnrealBuildTool;

public class SpellRiseServerTarget : TargetRules
{
	public SpellRiseServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_7;

		CppStandard = CppStandardVersion.Cpp20;

		// Opcional: monolithic no server dedicado
		LinkType = TargetLinkType.Monolithic;

		ExtraModuleNames.Add("SpellRise");
	}
}
