// Cabeçalho de módulo: configura o alvo Unreal e suas regras de build.
using UnrealBuildTool;

public class SpellRiseServerTarget : TargetRules
{
	public SpellRiseServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		ExtraModuleNames.Add("SpellRise");
	}
}
