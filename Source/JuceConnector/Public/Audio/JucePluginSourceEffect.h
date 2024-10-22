#pragma once

#include "CoreMinimal.h"
#include "Audio/JucePluginEffectProcessor.h"
#include "Sound/SoundEffectSource.h"
#include "Assets/JucePluginAsset.h"
#include "JucePluginSourceEffect.generated.h"

USTRUCT(BlueprintType)
struct JUCECONNECTOR_API FJucePluginSourceEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plugin")
	TObjectPtr<UJucePluginAsset> PluginAsset;
};

class JUCECONNECTOR_API FJucePluginSourceEffect final : public FSoundEffectSource
{
private:
	virtual void Init(const FSoundEffectSourceInitData& InInitData) override;
public:
	virtual void OnPresetChanged() override;
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;
private:
	float SampleRate{ 44100.0f };
	int NumChannel{ 2 };

	FJucePluginEffectProcessor EffectProcessor;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class JUCECONNECTOR_API UJucePluginSourceEffectPreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(JucePluginSourceEffect)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SourceEffectPreset)
	FJucePluginSourceEffectSettings Settings;
};
