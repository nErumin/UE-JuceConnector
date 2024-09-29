// Copyright Epic Games, Inc. All Rights Reserved.

#include "AudioPluginConnectorCommands.h"

#define LOCTEXT_NAMESPACE "FAudioPluginConnectorModule"

void FAudioPluginConnectorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "AudioPluginConnector", "Bring up AudioPluginConnector window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
