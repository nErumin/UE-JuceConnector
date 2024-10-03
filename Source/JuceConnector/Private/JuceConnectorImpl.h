#pragma once

#include "CoreMinimal.h"
#include "JuceConnectorModules.h"

class IAssetTypeActions;

class FJuceConnectorModule : public IJuceConnectorModule
{
public:
	virtual void ShutdownModule() override;
	virtual void StartupModule() override;
private:
	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

	void RegisterPropertyTypeCustomizations();
	void UnregisterPropertyTypeCustomizations();
private:
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;
};
