#pragma once

#include "CoreMinimal.h"

class IJucePluginFinder
{
public:
	IJucePluginFinder() = default;
	virtual ~IJucePluginFinder() = default;

	IJucePluginFinder(const IJucePluginFinder& Other) = default;
	IJucePluginFinder(IJucePluginFinder&& Other) noexcept = default;
protected:
	IJucePluginFinder& operator=(const IJucePluginFinder& Other) = default;
	IJucePluginFinder& operator=(IJucePluginFinder&& Other) noexcept = default;
public:
	virtual TArray<FString> FindPlugins(const TArray<FString>& PluginDirectories) const = 0;
};
