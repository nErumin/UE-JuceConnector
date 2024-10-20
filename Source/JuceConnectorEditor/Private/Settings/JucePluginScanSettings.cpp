#include "Settings/JucePluginScanSettings.h"

#define LOCTEXT_NAMESPACE "UJucePluginScanSettings"

FName FJucePluginDirectory::GetDirectoryPathPropertyName()
{
	return GET_MEMBER_NAME_CHECKED(FJucePluginDirectory, DirectoryPath);
}

FString FJucePluginDirectory::GetDirectoryPath() const
{
	const FString GameContentDirectoryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

	// Convert to absolute path.
	if (FPaths::IsRelative(DirectoryPath))
	{
		return FPaths::ConvertRelativePathToFull(GameContentDirectoryPath, DirectoryPath);
	}

	return DirectoryPath;
}

UJucePluginScanSettings* UJucePluginScanSettings::GetDefaultSettings()
{
	return GetMutableDefault<UJucePluginScanSettings>();
}

FName UJucePluginScanSettings::GetContainerName() const
{
	return FName{ "Project" };
}

FName UJucePluginScanSettings::GetCategoryName() const
{
	return FName{ "JuceConnector" };
}

FName UJucePluginScanSettings::GetSectionName() const
{
	return FName{ "ScanSettings" };
}

FText UJucePluginScanSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Scan Settings");
}

FText UJucePluginScanSettings::GetSectionDescription() const
{
	return LOCTEXT("Description", "Settings for default directories to find plugins.");
}

TArray<FString> UJucePluginScanSettings::GetVst3Directories() const
{
	TArray<FString> Directories;

	for (const FJucePluginDirectory& Directory : Vst3Directories)
	{
		if (const FString DirectoryPath = Directory.GetDirectoryPath(); !DirectoryPath.IsEmpty())
		{
			Directories.Add(DirectoryPath);
		}
	}

	return Directories;
}

#undef LOCTEXT_NAMESPACE
