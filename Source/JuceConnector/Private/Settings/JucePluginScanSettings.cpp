#include "Settings/JucePluginScanSettings.h"

#define LOCTEXT_NAMESPACE "UJucePluginScanSettings"

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

	for (const FDirectoryPath& DirectoryPath : Vst3Directories)
	{
		if (!DirectoryPath.Path.IsEmpty())
		{
			Directories.Add(DirectoryPath.Path);
		}
	}

	return Directories;
}

#undef LOCTEXT_NAMESPACE
