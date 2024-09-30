using System.IO;
using Microsoft.Extensions.Logging;
using UnrealBuildTool;

public class AudioPluginConnector : ModuleRules
{
	private string ThirdPartyDirectory => Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty"));

	public AudioPluginConnector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		InjectUnrealDependencies();
		InjectJuceDependencies();
	}

	private void InjectUnrealDependencies()
	{
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Projects",
			"InputCore",
			"EditorFramework",
			"UnrealEd",
			"ToolMenus",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
		});
	}

	private void InjectJuceDependencies()
	{
		/* RTTI is required to compile the JUCE source codes. */
		bUseRTTI = true;

		var juceDirectory = Path.Combine(ThirdPartyDirectory, "JUCE");

		/* Add the headers. */
		PrivateIncludePaths.Add(juceDirectory);

		/* Preprocessor settings -- This is an alternative for the AppConfig.h created from Projucer. */
		PrivateDefinitions.Add("JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1");
		PrivateDefinitions.Add("JUCE_STANDALONE_APPLICATION=0");

		PrivateDefinitions.Add("JUCE_MODULE_AVAILABLE_juce_core=1");
		PrivateDefinitions.Add("JUCE_MODULE_AVAILABLE_juce_data_structures=1");
		PrivateDefinitions.Add("JUCE_MODULE_AVAILABLE_juce_events=1");
		PrivateDefinitions.Add("JUCE_MODULE_AVAILABLE_juce_graphics=1");
	}
}
