#include "Audio/JucePluginSourceEffect.h"

#include "Engine/AssetManager.h"
#include "Juce/Plugins/JuceAudioProcessingHandle.h"
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

	AudioProcessingHandle.Reset();
	bProcessReadyFlag.clear();

	if (const UJuceHostedPluginAsset* Asset = Settings.PluginAsset.Get())
	{
		SetProcessingHandleFromAsset(Asset);
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this, ObjectPath = Settings.PluginAsset.ToSoftObjectPath()]
		{
			StreamableAssetHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad
			(
				ObjectPath,
				FStreamableDelegate::CreateRaw(this, &FJucePluginSourceEffect::OnPluginAssetLoadFinished)
			);
		});
	}
}

void FJucePluginSourceEffect::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	if (!bProcessReadyFlag.test())
	{
		// Sorry, we're not ready yet!
		FMemory::Memcpy(OutAudioBufferData, InData.InputSourceEffectBufferPtr, InData.NumSamples * sizeof(float));
		return;
	}

	if (AudioProcessingHandle)
	{
		if (!AudioProcessingHandle->IsPrepared())
		{
			AudioProcessingHandle->Prepare(SampleRate, NumChannel, InData.NumSamples);
		}

		AudioProcessingHandle->ProcessBlock
		(
			MakeArrayView(InData.InputSourceEffectBufferPtr, InData.NumSamples),
			MakeArrayView(OutAudioBufferData, InData.NumSamples)
		);
	}
}

void FJucePluginSourceEffect::OnPluginAssetLoadFinished()
{
	if (StreamableAssetHandle && StreamableAssetHandle->HasLoadCompleted())
	{
		const UJuceHostedPluginAsset* Asset = Cast<UJuceHostedPluginAsset>(StreamableAssetHandle->GetLoadedAsset());

		SetProcessingHandleFromAsset(Asset);
	}
}

void FJucePluginSourceEffect::SetProcessingHandleFromAsset(const UJuceHostedPluginAsset* Asset)
{
	if (!IsValid(Asset))
	{
		return;
	}

	if (const TSharedPtr<FJucePluginProxy> AliveProxy = Asset->GetPluginProxy().Pin())
	{
		JuceMessageUtils::ExecuteOnMessageThreadAsync([this, AliveProxy]() mutable
		{
			AudioProcessingHandle = AliveProxy->CreateNewProcessingHandle();
			bProcessReadyFlag.test_and_set();
		});
	}
}

