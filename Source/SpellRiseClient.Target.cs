// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpellRiseClientTarget : TargetRules
{
	public SpellRiseClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

		// ✅ Também evita depender de SpellRise.dll no client development
		LinkType = TargetLinkType.Monolithic;

		ExtraModuleNames.Add("SpellRise");
	}
}
