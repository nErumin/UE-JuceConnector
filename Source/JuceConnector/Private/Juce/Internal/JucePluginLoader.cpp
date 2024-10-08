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

TArray<FString> FJucePluginLoader::FindPlugins(const TArray<FString>& PluginDirectories) const
{
	const FString JoinedDirectories = FString::Join(PluginDirectories, TEXT(";"));
	const TArray<juce::PluginDescription> Descriptions = ScanDescriptions(JoinedDirectories);

	TArray<FString> PluginIdentifiers;

	for (const juce::PluginDescription& Description : Descriptions)
	{
		PluginIdentifiers.Add(JuceConverters::ToUnrealString(Description.fileOrIdentifier));
	}

	return PluginIdentifiers;
}

TOptional<juce::PluginDescription> FJucePluginLoader::FindDescription(const FString& PluginPath) const
{
	const FString PluginDirectory{ FPaths::GetPath(PluginPath) };

	for (const juce::PluginDescription& Description : ScanDescriptions(PluginDirectory))
	{
		const juce::String DescriptionFilePath = Description.fileOrIdentifier.replace("\\", "/");
		const juce::String SourceFilePath = JuceConverters::FromUnrealString(PluginPath).replace("\\", "/");

		if (DescriptionFilePath.equalsIgnoreCase(SourceFilePath))
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
