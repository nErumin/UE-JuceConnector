#include "Juce/Internal/JucePluginLoader.h"
#include "Juce/Utils/JuceConverters.h"
#include "JuceConnectorLogCategory.h"

FJucePluginLoader& FJucePluginLoader::Get()
{
	static FJucePluginLoader Loader;

	return Loader;
}

TUniquePtr<juce::AudioPluginInstance> FJucePluginLoader::LoadPlugin(const FString& PluginPath, const FJucePluginLoadOptions& Options)
{
	if (const TOptional<juce::PluginDescription> FoundDescription = FindDescription(PluginPath); FoundDescription.IsSet())
	{
		juce::String ErrorMessage;
		std::unique_ptr<juce::AudioPluginInstance> Instance = PluginFormatManager.createPluginInstance(*FoundDescription, Options.InitialSampleRate, Options.InitialBufferSize, ErrorMessage);

		if (ErrorMessage.isNotEmpty())
		{
			UE_LOG(LogJuceConnector, Error, TEXT("Error while creating plugin: %s"), *JuceConverters::ToUnrealString(ErrorMessage));
		}

		return TUniquePtr<juce::AudioPluginInstance>{ Instance.release() };
	}

	return nullptr;
}

TOptional<juce::PluginDescription> FJucePluginLoader::FindDescription(const FString& PluginPath) const
{
	const FString PluginDirectory{ FPaths::GetPath(PluginPath) };

	for (const juce::PluginDescription& Description : ScanDescriptions(PluginDirectory))
	{
		if (Description.fileOrIdentifier.equalsIgnoreCase(JuceConverters::FromUnrealString(PluginPath)))
		{
			return Description;
		}
	}

	return NullOpt;
}

TArray<juce::PluginDescription> FJucePluginLoader::ScanDescriptions(const FString& PluginDirectory) const
{
	juce::KnownPluginList PluginList;

	for (juce::AudioPluginFormat* Format : PluginFormatManager.getFormats())
	{
		juce::PluginDirectoryScanner Scanner
		{
			PluginList,
			*Format,
			juce::FileSearchPath{ *PluginDirectory },
			false,
			juce::File{},
			true
		};

		juce::String PluginName;

		while (Scanner.scanNextFile(true, PluginName))
		{
			/* Nothing to do */
		}
	}

	return JuceConverters::ToUnrealArray(PluginList.getTypes());
}

FJucePluginLoader::FJucePluginLoader()
{
	PluginFormatManager.addDefaultFormats();
}
