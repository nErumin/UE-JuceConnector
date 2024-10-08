#pragma once
#include "CoreMinimal.h"

class IJuceProcessorEditorHandle;

class FJucePluginProxy : public TSharedFromThis<FJucePluginProxy>
{
public:
	explicit FJucePluginProxy(const FString& PluginPath);
	~FJucePluginProxy();

	FJucePluginProxy(const FJucePluginProxy& Other) = delete;
	FJucePluginProxy(FJucePluginProxy&& Other) noexcept = default;

	FJucePluginProxy& operator=(const FJucePluginProxy& Other) = delete;
	FJucePluginProxy& operator=(FJucePluginProxy&& Other) noexcept = default;
public:
	TArray<FString> GetParameterNames() const;

	FText GetParameterValueAsText(const FString& ParameterName) const;
	float GetNormalizedParameterValue(const FString& ParameterName, float DefaultValue = 0.0f) const;
	FText GetNormalizedParameterValueAsText(const FString& ParameterName) const;

	void SetParameterValue(const FString& ParameterName, float NewValue);

	TSharedRef<IJuceProcessorEditorHandle> CreateEditorHandle();

	TArray<uint8> GetState() const;
	void SetState(const TArray<uint8>& StateMemoryBlock);
private:

private:
	struct FImpl;
	FImpl* Impl;
};
