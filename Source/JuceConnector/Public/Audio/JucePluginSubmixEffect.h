﻿#pragma once

#include "CoreMinimal.h"
#include "Audio/JucePluginEffectProcessor.h"
#include "Assets/JucePluginAsset.h"
#include "Sound/SoundEffectSubmix.h"
#include "JucePluginSubmixEffect.generated.h"

USTRUCT(BlueprintType)
struct JUCECONNECTOR_API FJucePluginSubmixEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plugin")
	TObjectPtr<UJucePluginAsset> PluginAsset;
};

class JUCECONNECTOR_API FJucePluginSubmixEffect final : public FSoundEffectSubmix
{
private:
	virtual void Init(const FSoundEffectSubmixInitData& InInitData) override;
public:
	virtual void OnPresetChanged() override;
	virtual void OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData) override;
private:
	float SampleRate{ 44100.0f };

	FJucePluginEffectProcessor EffectProcessor;
};

UCLASS(BlueprintType)
class JUCECONNECTOR_API UJucePluginSubmixEffectPreset : public USoundEffectSubmixPreset
{
	GENERATED_BODY()
public:
	EFFECT_PRESET_METHODS(JucePluginSubmixEffect)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SubmixEffectPreset)
	FJucePluginSubmixEffectSettings Settings;
};
