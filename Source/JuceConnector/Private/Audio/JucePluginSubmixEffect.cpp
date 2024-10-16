#include "Audio/JucePluginSubmixEffect.h"

#include "Engine/AssetManager.h"
#include "Juce/Plugins/JuceAudioProcessingHandle.h"
#include "Juce/Utils/JuceMessageUtils.h"

void FJucePluginSubmixEffect::Init(const FSoundEffectSubmixInitData& InInitData)
{
	SampleRate = InInitData.SampleRate;
}

void FJucePluginSubmixEffect::OnPresetChanged()
{
	ensure(!IsInGameThread());

	GET_EFFECT_SETTINGS(JucePluginSubmixEffect)

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
				FStreamableDelegate::CreateRaw(this, &FJucePluginSubmixEffect::OnPluginAssetLoadFinished)
			);
		});
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

void FJucePluginSubmixEffect::OnPluginAssetLoadFinished()
{
	if (StreamableAssetHandle && StreamableAssetHandle->HasLoadCompleted())
	{
		const UJuceHostedPluginAsset* Asset = Cast<UJuceHostedPluginAsset>(StreamableAssetHandle->GetLoadedAsset());

		SetProcessingHandleFromAsset(Asset);
	}
}

void FJucePluginSubmixEffect::SetProcessingHandleFromAsset(const UJuceHostedPluginAsset* Asset)
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

