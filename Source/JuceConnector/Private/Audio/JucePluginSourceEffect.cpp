#include "Audio/JucePluginSourceEffect.h"
#include "Audio/JucePluginEffectProcessor.h"
#include "Juce/Utils/JuceMessageUtils.h"

void FJucePluginSourceEffect::Init(const FSoundEffectSourceInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
	NumChannel = InInitData.NumSourceChannels;
}

void FJucePluginSourceEffect::OnPresetChanged()
{
	ensure(!IsInGameThread());
	GET_EFFECT_SETTINGS(JucePluginSourceEffect)

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

void FJucePluginSourceEffect::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	if (!EffectProcessor.HasProcessingHandle())
	{
		// Sorry, we're not ready yet!
		FMemory::Memcpy(OutAudioBufferData, InData.InputSourceEffectBufferPtr, InData.NumSamples * sizeof(float));
		return;
	}

	EffectProcessor.ProcessBlock
	(
		MakeArrayView(InData.InputSourceEffectBufferPtr, InData.NumSamples),
		MakeArrayView(OutAudioBufferData, InData.NumSamples),
		FJucePluginEffectProcessContext{ .SampleRate = SampleRate, .NumChannel = NumChannel }
	);
}
