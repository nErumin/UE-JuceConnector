#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FJucePluginParameterChanged, const FString&, float);

class IJuceProcessorEditorHandle;
class IJuceAudioProcessingHandle;

class IJucePluginProxy : public TSharedFromThis<IJucePluginProxy>
{
public:
	IJucePluginProxy() = default;
	virtual ~IJucePluginProxy() = default;

	IJucePluginProxy(const IJucePluginProxy& Other) = default;
	IJucePluginProxy(IJucePluginProxy&& Other) noexcept = default;
protected:
	IJucePluginProxy& operator=(const IJucePluginProxy& Other) = default;
	IJucePluginProxy& operator=(IJucePluginProxy&& Other) noexcept = default;
public:
	virtual TArray<FString> GetParameterNames() const = 0;

	virtual FText GetParameterValueAsText(const FString& ParameterName) const = 0;
	virtual float GetNormalizedParameterValue(const FString& ParameterName, float DefaultValue) const = 0;
	virtual FText GetNormalizedParameterValueAsText(const FString& ParameterName) const = 0;

	virtual void SetParameterValue(const FString& ParameterName, float NewValue) = 0;
	virtual FJucePluginParameterChanged& OnPluginParameterChanged() = 0;

	virtual TSharedRef<IJuceProcessorEditorHandle> GetEditorHandle() const = 0;
	virtual TWeakPtr<IJuceAudioProcessingHandle> BorrowProcessingHandle() const = 0;

	virtual TArray<uint8> GetState() const = 0;
	virtual void SetState(const TArray<uint8>& StateMemoryBlock) = 0;
};

