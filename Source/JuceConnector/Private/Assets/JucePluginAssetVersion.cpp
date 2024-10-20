#include "Assets/JucePluginAssetVersion.h"

const FGuid FJucePluginAssetVersion::GUID{ 0xE01FD768, 0x4B4A237A, 0x4C4AB287, 0xD0564057};

FCustomVersionRegistration GJucePluginAssetVersionRegistration{ FJucePluginAssetVersion::GUID, FJucePluginAssetVersion::LatestVersion, TEXT("JuceConnector-HostedPluginAsset") };
