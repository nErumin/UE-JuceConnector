#include "Audio/JucePluginEffectProcessor.h"

#include "Juce/Utils/JuceMessageUtils.h"
#include "Assets/JuceHostedPluginAsset.h"
#include "Engine/AssetManager.h"
#include "Juce/Plugins/JuceAudioProcessingHandle.h"

FJucePluginEffectProcessor::~FJucePluginEffectProcessor()
{
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [Handle = StreamableAssetHandle]
		{
			Handle->CancelHandle();
		});
	}
}

void FJucePluginEffectProcessor::PrepareProcess(const TSoftObjectPtr<UJuceHostedPluginAsset>& InAsset)
{
	SoftAsset = InAsset;

	AudioProcessingHandle.Reset();
	bProcessReadyFlag.clear();

	if (const UJuceHostedPluginAsset* AliveAsset = SoftAsset.Get())
	{
		SetProcessingHandleFromAsset(AliveAsset);
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this, ObjectPath = SoftAsset.ToSoftObjectPath()]
		{
			StreamableAssetHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad
			(
				ObjectPath,
				FStreamableDelegate::CreateSP(this, &FJucePluginEffectProcessor::OnPluginAssetLoadFinished)
			);
		});
	}
}

bool FJucePluginEffectProcessor::IsPrepared() const
{
	return bProcessReadyFlag.test() && AudioProcessingHandle;
}

void FJucePluginEffectProcessor::ProcessBlock(const TArrayView<float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context)
{
	if (IsPrepared())
	{
		if (!AudioProcessingHandle->IsPrepared())
		{
			AudioProcessingHandle->Prepare(Context.SampleRate, Context.NumChannel, InputBuffer.Num());
		}

		AudioProcessingHandle->ProcessBlock(InputBuffer, OutputBuffer);
	}
}

void FJucePluginEffectProcessor::OnPluginAssetLoadFinished()
{
	if (StreamableAssetHandle && StreamableAssetHandle->HasLoadCompleted())
	{
		const UJuceHostedPluginAsset* Asset = Cast<UJuceHostedPluginAsset>(StreamableAssetHandle->GetLoadedAsset());
		SetProcessingHandleFromAsset(Asset);
	}
}

void FJucePluginEffectProcessor::SetProcessingHandleFromAsset(const UJuceHostedPluginAsset* Asset)
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
