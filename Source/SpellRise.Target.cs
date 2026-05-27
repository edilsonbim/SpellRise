// Cabeçalho de módulo: configura o alvo Unreal e suas regras de build.
using UnrealBuildTool;

public class SpellRiseTarget : TargetRules
{
	public SpellRiseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;


		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_7;


		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("SpellRise");
	}
}
