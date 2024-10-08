#include "Assets/JuceHostedPluginAssetVersion.h"

const FGuid FJuceHostedPluginAssetVersion::GUID{ 0xE01FD768, 0x4B4A237A, 0x4C4AB287, 0xD0564057};

FCustomVersionRegistration GJuceHostedPluginAssetVersionRegistration{ FJuceHostedPluginAssetVersion::GUID, FJuceHostedPluginAssetVersion::LatestVersion, TEXT("JuceConnector-HostedPluginAsset") };
