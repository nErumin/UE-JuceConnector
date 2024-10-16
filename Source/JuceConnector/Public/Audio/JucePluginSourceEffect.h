#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundEffectSource.h"
#include "Assets/JuceHostedPluginAsset.h"
#include "Engine/StreamableManager.h"
#include "JucePluginSourceEffect.generated.h"

USTRUCT(BlueprintType)
struct JUCECONNECTOR_API FJucePluginSourceEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plugin")
	TSoftObjectPtr<UJuceHostedPluginAsset> PluginAsset;
};

class JUCECONNECTOR_API FJucePluginSourceEffect final : public FSoundEffectSource
{
private:
	virtual void Init(const FSoundEffectSourceInitData& InInitData) override;
public:
	virtual void OnPresetChanged() override;
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;
private:
	void OnPluginAssetLoadFinished();

	void SetProcessingHandleFromAsset(const UJuceHostedPluginAsset* Asset);
private:
	float SampleRate{ 44100.0f };
	int NumChannel{ 2 };

	TSharedPtr<IJuceAudioProcessingHandle> AudioProcessingHandle{ nullptr };
	TSharedPtr<FStreamableHandle> StreamableAssetHandle{ nullptr };
	std::atomic_flag bProcessReadyFlag;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class JUCECONNECTOR_API UJucePluginSourceEffectPreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(JucePluginSourceEffect)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SubmixEffectPreset)
	FJucePluginSourceEffectSettings Settings;
};
