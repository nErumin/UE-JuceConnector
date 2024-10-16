#pragma once
#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"

class UJuceHostedPluginAsset;
class IJuceAudioProcessingHandle;

struct JUCECONNECTOR_API FJucePluginEffectProcessContext
{
	float SampleRate{ 44100.0 };
	int NumChannel{ 2 };
};

class JUCECONNECTOR_API FJucePluginEffectProcessor : public TSharedFromThis<FJucePluginEffectProcessor>
{
public:
	~FJucePluginEffectProcessor();
public:
	void PrepareProcess(const TSoftObjectPtr<UJuceHostedPluginAsset>& InAsset);
	bool IsPrepared() const;

	void ProcessBlock(const TArrayView<float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context = FJucePluginEffectProcessContext{});
private:
	void OnPluginAssetLoadFinished();
	void SetProcessingHandleFromAsset(const UJuceHostedPluginAsset* Asset);
private:
	TSoftObjectPtr<UJuceHostedPluginAsset> SoftAsset;

	TSharedPtr<IJuceAudioProcessingHandle> AudioProcessingHandle{ nullptr };
	TSharedPtr<FStreamableHandle> StreamableAssetHandle{ nullptr };
	std::atomic_flag bProcessReadyFlag;
};
