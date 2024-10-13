#include "Audio/JucePluginSubmixEffect.h"

#include "JuceConnectorLogCategory.h"
#include "Juce/Plugins/JuceAudioProcessingHandle.h"

void FJucePluginSubmixEffect::Init(const FSoundEffectSubmixInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
}

void FJucePluginSubmixEffect::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(JucePluginSubmixEffect)

	AudioProcessingHandle.Reset();

	if (const UJuceHostedPluginAsset* Asset = Settings.PluginAsset.Get())
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = Asset->GetPluginProxy().Pin())
		{
			AudioProcessingHandle = AliveProxy->CreateNewProcessingHandle();
		}
	}
}

void FJucePluginSubmixEffect::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	if (AudioProcessingHandle)
	{
		if (!AudioProcessingHandle->IsPrepared())
		{
			AudioProcessingHandle->Prepare(SampleRate, InData.NumChannels, InData.NumFrames);
		}

		AudioProcessingHandle->ProcessBlock(InData.AudioBuffer, OutData.AudioBuffer);
	}
}

