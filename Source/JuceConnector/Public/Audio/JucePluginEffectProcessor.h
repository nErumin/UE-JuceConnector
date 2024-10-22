#pragma once
#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"

class UJucePluginAsset;
class IJuceAudioProcessingHandle;

struct JUCECONNECTOR_API FJucePluginEffectProcessContext
{
	float SampleRate{ 44100.0 };
	int NumChannel{ 2 };
};

class JUCECONNECTOR_API FJucePluginEffectProcessor : public TSharedFromThis<FJucePluginEffectProcessor>
{
public:
	void SetProcessingHandle(const TWeakPtr<IJuceAudioProcessingHandle>& InWeakHandle);
	bool HasProcessingHandle() const;

	void ProcessBlock(const TArrayView<const float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context = FJucePluginEffectProcessContext{});
	void Reset();
private:
	TWeakPtr<IJuceAudioProcessingHandle> WeakProcessingHandle{ nullptr };
};
