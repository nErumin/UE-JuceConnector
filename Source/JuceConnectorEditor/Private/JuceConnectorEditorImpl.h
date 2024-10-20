#pragma once

#include "CoreMinimal.h"
#include "JuceConnectorEditorModules.h"

class IAssetTypeActions;

class FJuceConnectorEditorImpl final : public IJuceConnectorEditorModules
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

	void RegisterTypeCustomizations();
	void UnregisterTypeCustomizations();

	void RegisterMetasoundPinNodes();
private:
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;
};
