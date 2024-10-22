#pragma once

#include "CoreMinimal.h"
#include "Juce/JucePluginProxy.h"

class FJucePluginProxy final : public IJucePluginProxy
{
public:
	explicit FJucePluginProxy(const FString& PluginPath);
	virtual ~FJucePluginProxy() override;

	FJucePluginProxy(const FJucePluginProxy& Other) = delete;
	FJucePluginProxy& operator=(const FJucePluginProxy& Other) = delete;
public:
	virtual TArray<FString> GetParameterNames() const override;

	virtual FText GetParameterValueAsText(const FString& ParameterName) const override;
	virtual float GetNormalizedParameterValue(const FString& ParameterName, float DefaultValue = 0.0f) const override;
	virtual FText GetNormalizedParameterValueAsText(const FString& ParameterName) const override;

	virtual void SetParameterValue(const FString& ParameterName, float NewValue) override;

	virtual FJucePluginParameterChanged& OnPluginParameterChanged() override;

	virtual TSharedRef<IJuceProcessorEditorHandle> GetEditorHandle() const override;
	virtual TWeakPtr<IJuceAudioProcessingHandle> BorrowProcessingHandle() const override;

	virtual TArray<uint8> GetState() const override;
	virtual void SetState(const TArray<uint8>& StateMemoryBlock) override;
private:
	struct FImpl;
	FImpl* Impl;
};
