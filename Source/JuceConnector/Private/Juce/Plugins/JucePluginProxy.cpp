#include "Juce/Plugins/JucePluginProxy.h"
#include "Juce/Internal/JucePluginLoader.h"
#include "Juce/JuceHeader.h"
#include "Juce/Utils/JuceConverters.h"

struct FJucePluginProxy::FImpl
{
public:
	explicit FImpl(const FString& InPluginPath)
		: PluginPath{ InPluginPath }
	{
	}
public:
	juce::AudioPluginInstance* GetPluginInstance() const
	{
		if (!PluginInstance)
		{
			PluginInstance = FJucePluginLoader::Get().LoadPlugin(PluginPath);
		}

		return PluginInstance.Get();
	}
private:
	FString PluginPath;
	mutable TUniquePtr<juce::AudioPluginInstance> PluginInstance{ nullptr };
};

FJucePluginProxy::FJucePluginProxy(const FString& PluginPath)
	: Impl{ new FImpl(PluginPath) }
{
	check(Impl);
}

FJucePluginProxy::~FJucePluginProxy()
{
	delete Impl;
	Impl = nullptr;
}

TArray<FString> FJucePluginProxy::GetParameterNames() const
{
	TArray<FString> Names;

	if (const juce::AudioPluginInstance* Instance = Impl->GetPluginInstance())
	{
		for (const juce::AudioProcessorParameter* Parameter : Instance->getParameters())
		{
			Names.Add(JuceConverters::ToUnrealString(Parameter->getName(1024)));
		}
	}

	return Names;
}

TArray<uint8> FJucePluginProxy::GetState() const
{
	if (juce::AudioPluginInstance* Instance = Impl->GetPluginInstance())
	{
		juce::MemoryBlock JuceBlock;
		Instance->getStateInformation(JuceBlock);

		return TArray{ static_cast<uint8*>(JuceBlock.getData()), static_cast<int32>(JuceBlock.getSize()) };
	}

	return TArray<uint8>{};
}

void FJucePluginProxy::SetState(const TArray<uint8>& StateMemoryBlock)
{
	if (juce::AudioPluginInstance* Instance = Impl->GetPluginInstance())
	{
		Instance->setStateInformation(StateMemoryBlock.GetData(), StateMemoryBlock.Num());
	}
}
