#include "Audio/JucePluginSubmixEffect.h"
#include "Juce/Utils/JuceMessageUtils.h"

void FJucePluginSubmixEffect::Init(const FSoundEffectSubmixInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
}

void FJucePluginSubmixEffect::OnPresetChanged()
{
	ensure(!IsInGameThread());
	GET_EFFECT_SETTINGS(JucePluginSubmixEffect)

	EffectProcessor.SetProcessingHandle(nullptr);

	if (!Settings.PluginAsset)
	{
		return;
	}

	JuceMessageUtils::ExecuteOnMessageThread([this, PluginAsset = Settings.PluginAsset]
	{
		if (const TSharedPtr<IJucePluginProxy> AliveProxy = PluginAsset->GetPluginProxy().Pin())
		{
			EffectProcessor.SetProcessingHandle(AliveProxy->BorrowProcessingHandle());
		}
	});
}

void FJucePluginSubmixEffect::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	if (!EffectProcessor.HasProcessingHandle())
	{
		// Sorry, we're not ready yet!
		FMemory::Memcpy(OutData.AudioBuffer->GetData(), InData.AudioBuffer->GetData(), InData.AudioBuffer->Num() * sizeof(float));
		return;
	}

	EffectProcessor.ProcessBlock
	(
		MakeArrayView(InData.AudioBuffer->GetData(), InData.NumFrames * InData.NumChannels),
		MakeArrayView(OutData.AudioBuffer->GetData(), InData.NumFrames * InData.NumChannels),
		FJucePluginEffectProcessContext{ .SampleRate = SampleRate, .NumChannel = InData.NumChannels }
	);
}
