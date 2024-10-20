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
	~FJucePluginEffectProcessor();
public:
	void PrepareProcess(const TSoftObjectPtr<UJucePluginAsset>& InAsset);
	bool IsPrepared() const;

	void ProcessBlock(const TArrayView<const float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context = FJucePluginEffectProcessContext{});
	void Reset();
private:
	void OnPluginAssetLoadFinished();
	void SetProcessingHandleFromAsset(const UJucePluginAsset* Asset);
private:
	TSoftObjectPtr<UJucePluginAsset> SoftAsset;

	TSharedPtr<IJuceAudioProcessingHandle> AudioProcessingHandle{ nullptr };
	TSharedPtr<FStreamableHandle> StreamableAssetHandle{ nullptr };
	std::atomic_flag bProcessReadyFlag;
};
