// Cabeçalho de módulo: configura o alvo Unreal e suas regras de build.
using UnrealBuildTool;

public class SpellRiseEditorTarget : TargetRules
{
	public SpellRiseEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		bOverrideBuildEnvironment = true;

		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_8;

		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("SpellRise");
	}
}
