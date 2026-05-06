// Cabeçalho de módulo: configura o alvo Unreal e suas regras de build.
using UnrealBuildTool;
using System.IO;

public class SpellRise : ModuleRules
{
	public SpellRise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = true;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ApplicationCore",
				"PhysicsCore",
				"InputCore",
				"EnhancedInput",
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				"UMG",
				"Niagara",
				"ModularGameplay",
				"NarrativeInventory",
				"NarrativeEquipment",
				"NarrativeInteraction",
				"NarrativeNavigator"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{

				"Slate",
				"SlateCore",


				"RenderCore",
				"RHI",


				"NetCore",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"OnlineSubsystemSteam",


				"Projects",
				"DeveloperSettings",
				"Json",
				"JsonUtilities",


				"AudioMixer",
				"AIModule"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Type == TargetType.Server)
		{
			string projectRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
			string steamRedistDir = Path.Combine(projectRoot, "Build", "SteamRedist", "Win64");
			string steamAppIdPath = Path.Combine(projectRoot, "Build", "steam_appid.txt");

			string steamApiPath = Path.Combine(steamRedistDir, "steam_api64.dll");
			string steamClientPath = Path.Combine(steamRedistDir, "steamclient64.dll");
			string tier0Path = Path.Combine(steamRedistDir, "tier0_s64.dll");
			string vstdlibPath = Path.Combine(steamRedistDir, "vstdlib_s64.dll");

			if (!File.Exists(steamApiPath) || !File.Exists(steamClientPath) || !File.Exists(tier0Path) || !File.Exists(vstdlibPath))
			{
				throw new BuildException("Steam redistributables ausentes em Build/SteamRedist/Win64. Arquivos obrigatorios: steam_api64.dll, steamclient64.dll, tier0_s64.dll, vstdlib_s64.dll");
			}

			if (!File.Exists(steamAppIdPath))
			{
				throw new BuildException("Arquivo obrigatorio ausente: Build/steam_appid.txt");
			}

			RuntimeDependencies.Add("$(TargetOutputDir)/steam_api64.dll", steamApiPath, StagedFileType.NonUFS);
			RuntimeDependencies.Add("$(TargetOutputDir)/steamclient64.dll", steamClientPath, StagedFileType.NonUFS);
			RuntimeDependencies.Add("$(TargetOutputDir)/tier0_s64.dll", tier0Path, StagedFileType.NonUFS);
			RuntimeDependencies.Add("$(TargetOutputDir)/vstdlib_s64.dll", vstdlibPath, StagedFileType.NonUFS);
			RuntimeDependencies.Add("$(TargetOutputDir)/steam_appid.txt", steamAppIdPath, StagedFileType.NonUFS);
		}
	}
}
