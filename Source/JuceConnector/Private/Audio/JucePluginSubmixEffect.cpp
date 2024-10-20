#include "Audio/JucePluginSubmixEffect.h"
#include "Juce/Utils/JuceMessageUtils.h"

void FJucePluginSubmixEffect::Init(const FSoundEffectSubmixInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;

	EffectProcessor = MakeShared<FJucePluginEffectProcessor>();
}

void FJucePluginSubmixEffect::OnPresetChanged()
{
	ensure(!IsInGameThread());
	GET_EFFECT_SETTINGS(JucePluginSubmixEffect)

	EffectProcessor->PrepareProcess(Settings.PluginAsset);
}

void FJucePluginSubmixEffect::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	if (!EffectProcessor || !EffectProcessor->IsPrepared())
	{
		// Sorry, we're not ready yet!
		FMemory::Memcpy(OutData.AudioBuffer->GetData(), InData.AudioBuffer->GetData(), InData.AudioBuffer->Num() * sizeof(float));
		return;
	}

	EffectProcessor->ProcessBlock
	(
		MakeArrayView(InData.AudioBuffer->GetData(), InData.NumFrames * InData.NumChannels),
		MakeArrayView(OutData.AudioBuffer->GetData(), InData.NumFrames * InData.NumChannels),
		FJucePluginEffectProcessContext{ .SampleRate = SampleRate, .NumChannel = InData.NumChannels }
	);
}
