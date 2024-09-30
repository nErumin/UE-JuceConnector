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

TConstArrayView<FDirectoryPath> UJucePluginScanSettings::GetVst3Directories() const
{
	return Vst3Directories;
}

#undef LOCTEXT_NAMESPACE
