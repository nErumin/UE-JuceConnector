#pragma once

#include "CoreMinimal.h"
#include "JuceConnectorStyle.h"
#include "Framework/Commands/Commands.h"

class FJuceConnectorCommands : public TCommands<FJuceConnectorCommands>
{
public:

	FJuceConnectorCommands()
		: TCommands<FJuceConnectorCommands>(TEXT("AudioPluginConnector"), NSLOCTEXT("Contexts", "AudioPluginConnector", "AudioPluginConnector Plugin"), NAME_None, FJuceConnectorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
