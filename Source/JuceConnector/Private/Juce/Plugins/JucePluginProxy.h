#pragma once
#include "CoreMinimal.h"

class FJucePluginProxy
{
public:
	explicit FJucePluginProxy(const FString& PluginPath);
	~FJucePluginProxy();

	FJucePluginProxy(const FJucePluginProxy& Other) = delete;
	FJucePluginProxy& operator=(const FJucePluginProxy& Other) = delete;
	FJucePluginProxy(FJucePluginProxy&& Other) noexcept = default;
	FJucePluginProxy& operator=(FJucePluginProxy&& Other) noexcept = default;
public:
	TArray<FString> GetParameterNames() const;

	TArray<uint8> GetState() const;
	void SetState(const TArray<uint8>& StateMemoryBlock);
private:
	struct FImpl;
	FImpl* Impl;
};
