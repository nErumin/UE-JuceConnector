#pragma once

#include "CoreMinimal.h"
#include "Juce/JuceHeader.h"

struct FJucePluginLoadOptions
{
	double InitialSampleRate = 44100.0;
	int InitialBufferSize = 1024;
};

class FJucePluginLoader
{
public:
	static FJucePluginLoader& Get();
public:
	TUniquePtr<juce::AudioPluginInstance> LoadPlugin(const FString& PluginPath, const FJucePluginLoadOptions& Options = FJucePluginLoadOptions{});
private:
	TOptional<juce::PluginDescription> FindDescription(const FString& PluginPath) const;
	TArray<juce::PluginDescription> ScanDescriptions(const FString& PluginDirectory) const;
private:
	FJucePluginLoader();
	~FJucePluginLoader() = default;

	FJucePluginLoader(const FJucePluginLoader& Other) = delete;
	FJucePluginLoader& operator=(const FJucePluginLoader& Other) = delete;
	FJucePluginLoader(FJucePluginLoader&& Other) noexcept = delete;
	FJucePluginLoader& operator=(FJucePluginLoader&& Other) noexcept = delete;
private:
	juce::AudioPluginFormatManager PluginFormatManager;
};
