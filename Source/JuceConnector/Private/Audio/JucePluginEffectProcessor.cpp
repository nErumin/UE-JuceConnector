#include "Audio/JucePluginEffectProcessor.h"

#include "Juce/Utils/JuceMessageUtils.h"
#include "Assets/JucePluginAsset.h"
#include "Engine/AssetManager.h"
#include "Juce/JuceAudioProcessingHandle.h"

FJucePluginEffectProcessor::~FJucePluginEffectProcessor()
{
	if (!IsInGameThread() && StreamableAssetHandle)
	{
		AsyncTask(ENamedThreads::GameThread, [Handle = StreamableAssetHandle]
		{
			Handle->CancelHandle();
		});
	}
}

void FJucePluginEffectProcessor::PrepareProcess(const TSoftObjectPtr<UJucePluginAsset>& InAsset)
{
	SoftAsset = InAsset;

	AudioProcessingHandle.Reset();
	bProcessReadyFlag.clear();

	if (const UJucePluginAsset* AliveAsset = SoftAsset.Get())
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

void FJucePluginEffectProcessor::ProcessBlock(const TArrayView<const float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context)
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

void FJucePluginEffectProcessor::Reset()
{
	AudioProcessingHandle->Reset();
}

void FJucePluginEffectProcessor::OnPluginAssetLoadFinished()
{
	if (StreamableAssetHandle && StreamableAssetHandle->HasLoadCompleted())
	{
		const UJucePluginAsset* Asset = Cast<UJucePluginAsset>(StreamableAssetHandle->GetLoadedAsset());
		SetProcessingHandleFromAsset(Asset);
	}
}

void FJucePluginEffectProcessor::SetProcessingHandleFromAsset(const UJucePluginAsset* Asset)
{
	if (!IsValid(Asset))
	{
		return;
	}

	if (const TSharedPtr<IJucePluginProxy> AliveProxy = Asset->GetPluginProxy().Pin())
	{
		JuceMessageUtils::ExecuteOnMessageThreadAsync([this, AliveProxy]() mutable
		{
			AudioProcessingHandle = AliveProxy->CreateNewProcessingHandle();
			bProcessReadyFlag.test_and_set();
		});
	}
}
