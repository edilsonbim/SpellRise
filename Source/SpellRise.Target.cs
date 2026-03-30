// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpellRiseTarget : TargetRules
{
	public SpellRiseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		// ✅ Padronizar UE 5.7
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion  = EngineIncludeOrderVersion.Unreal5_7;

		// ✅ Evita símbolos "inline/const" quebrando link quando algo força std antigo
		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("SpellRise");
	}
}
