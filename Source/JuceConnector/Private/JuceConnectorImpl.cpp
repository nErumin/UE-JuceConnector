#include "JuceConnectorImpl.h"

#include "MetasoundDataTypeRegistrationMacro.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "Audio/JucePluginMetasoundNode.h"
#include "Assets/JucePluginAsset.h"

#include "Juce/JuceHeader.h"
#include "Juce/JucePluginLoader.h"

#define LOCTEXT_NAMESPACE "FJuceConnectorModule"

void FJuceConnectorModule::StartupModule()
{
	// Use the game thread as message thread.
	juce::MessageManager::getInstance();

	RegisterAudioProcessTypes();
}

IJucePluginFinder& FJuceConnectorModule::GetPluginFinder()
{
	return FJucePluginLoader::Get();
}

void FJuceConnectorModule::ShutdownModule()
{
}

void FJuceConnectorModule::RegisterAudioProcessTypes()
{
	FMetasoundFrontendRegistryContainer::Get()->EnqueueInitCommand([]
	{
		Metasound::RegisterNodeWithFrontend<JuceConnectorMetasound::FProcessorNode>();
	});

	FMetasoundFrontendRegistryContainer::Get()->EnqueueInitCommand([]
	{
		Metasound::RegisterDataTypeWithFrontend<JuceConnectorMetasound::FJucePluginAssetData, Metasound::ELiteralType::UObjectProxy, UJucePluginAsset>();
	});

	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJuceConnectorModule, JuceConnector)
