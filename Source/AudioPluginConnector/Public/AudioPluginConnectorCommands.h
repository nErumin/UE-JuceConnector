#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AudioPluginConnectorStyle.h"

class FAudioPluginConnectorCommands : public TCommands<FAudioPluginConnectorCommands>
{
public:

	FAudioPluginConnectorCommands()
		: TCommands<FAudioPluginConnectorCommands>(TEXT("AudioPluginConnector"), NSLOCTEXT("Contexts", "AudioPluginConnector", "AudioPluginConnector Plugin"), NAME_None, FAudioPluginConnectorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
