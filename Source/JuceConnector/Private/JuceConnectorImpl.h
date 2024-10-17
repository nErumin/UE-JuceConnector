#pragma once

#include "CoreMinimal.h"
#include "JuceConnectorModules.h"

class IAssetTypeActions;

class FJuceConnectorModule final : public IJuceConnectorModule
{
public:
	virtual void ShutdownModule() override;
	virtual void StartupModule() override;
private:
	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

	void RegisterTypeCustomizations();
	void UnregisterTypeCustomizations();

	void RegisterAudioProcessTypes();
private:
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;
};
