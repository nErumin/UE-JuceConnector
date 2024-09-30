#include "JuceConnectorCommands.h"

#define LOCTEXT_NAMESPACE "FAudioPluginConnectorModule"

void FJuceConnectorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "AudioPluginConnector", "Bring up AudioPluginConnector window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
