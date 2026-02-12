// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpellRiseServerTarget : TargetRules
{
	public SpellRiseServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		// ✅ Evita depender de SpellRise.dll (monolithic)
		LinkType = TargetLinkType.Monolithic;

		ExtraModuleNames.Add("SpellRise");
	}
}
