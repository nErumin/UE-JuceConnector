#include "Audio/JucePluginSubmixEffect.h"

#include "Juce/Plugins/JuceAudioProcessingHandle.h"
#include "Juce/Utils/JuceMessageUtils.h"

void FJucePluginSubmixEffect::Init(const FSoundEffectSubmixInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
}

void FJucePluginSubmixEffect::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(JucePluginSubmixEffect)

	AudioProcessingHandle.Reset();
	bProcessReadyFlag.clear();

	if (const UJuceHostedPluginAsset* Asset = Settings.PluginAsset.Get())
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = Asset->GetPluginProxy().Pin())
		{
			JuceMessageUtils::ExecuteOnMessageThreadAsync([this, AliveProxy]() mutable
			{
				AudioProcessingHandle = AliveProxy->CreateNewProcessingHandle();
				bProcessReadyFlag.test_and_set();
			});
		}
	}
}

void FJucePluginSubmixEffect::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	if (!bProcessReadyFlag.test())
	{
		// Sorry, we're not ready yet!
		FMemory::Memcpy(OutData.AudioBuffer->GetData(), InData.AudioBuffer->GetData(), InData.AudioBuffer->Num() * sizeof(float));
		return;
	}

	if (AudioProcessingHandle)
	{
		if (!AudioProcessingHandle->IsPrepared())
		{
			AudioProcessingHandle->Prepare(SampleRate, InData.NumChannels, InData.NumFrames);
		}

		AudioProcessingHandle->ProcessBlock(InData.AudioBuffer, OutData.AudioBuffer);
	}
}

