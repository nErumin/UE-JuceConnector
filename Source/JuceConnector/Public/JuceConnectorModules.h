#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IJucePluginFinder;

class IJuceConnectorModule : public IModuleInterface
{
public:
	virtual IJucePluginFinder& GetPluginFinder() = 0;
};
