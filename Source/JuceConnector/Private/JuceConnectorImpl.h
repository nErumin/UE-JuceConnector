#pragma once

#include "CoreMinimal.h"
#include "JuceConnectorModules.h"

class IAssetTypeActions;

class FJuceConnectorModule final : public IJuceConnectorModule
{
public:
	virtual void ShutdownModule() override;
	virtual void StartupModule() override;

	virtual IJucePluginFinder& GetPluginFinder() override;
private:
	void RegisterAudioProcessTypes();
};
