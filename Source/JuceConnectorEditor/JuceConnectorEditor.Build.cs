using System.IO;
using UnrealBuildTool;

public class JuceConnectorEditor : ModuleRules
{
	public JuceConnectorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"DeveloperSettings",
			"JuceConnector"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"CoreUObject",
			"Projects",
			"EditorFramework",
			"UnrealEd",
			"Engine",
			"Slate",
			"SlateCore",
			"MetasoundEditor"
		});
	}
}
