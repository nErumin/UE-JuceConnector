#include "Audio/JucePluginEffectProcessor.h"

#include "Juce/Utils/JuceMessageUtils.h"
#include "Assets/JucePluginAsset.h"
#include "Juce/JuceAudioProcessingHandle.h"

void FJucePluginEffectProcessor::SetProcessingHandle(const TWeakPtr<IJuceAudioProcessingHandle>& InWeakHandle)
{
	WeakProcessingHandle = InWeakHandle;
}

bool FJucePluginEffectProcessor::HasProcessingHandle() const
{
	return WeakProcessingHandle.IsValid();
}

void FJucePluginEffectProcessor::ProcessBlock(const TArrayView<const float>& InputBuffer, const TArrayView<float>& OutputBuffer, const FJucePluginEffectProcessContext& Context)
{
	if (TSharedPtr<IJuceAudioProcessingHandle> AliveProcessingHandle = WeakProcessingHandle.Pin())
	{
		if (!AliveProcessingHandle->IsPrepared())
		{
			AliveProcessingHandle->Prepare(Context.SampleRate, Context.NumChannel, InputBuffer.Num());
		}

		AliveProcessingHandle->ProcessBlock(InputBuffer, OutputBuffer);
	}
}

void FJucePluginEffectProcessor::Reset()
{
	if (TSharedPtr<IJuceAudioProcessingHandle> AliveProcessingHandle = WeakProcessingHandle.Pin())
	{
		AliveProcessingHandle->Reset();
	}
}
