// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpellRiseTarget : TargetRules
{
	public SpellRiseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SpellRise");
	}
}
