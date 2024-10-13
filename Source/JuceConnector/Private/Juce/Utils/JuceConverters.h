#pragma once

#include "CoreMinimal.h"
#include "Juce/JuceHeader.h"

namespace JuceConverters
{
	inline FString ToUnrealString(const juce::String& JuceString)
	{
		return FString{ JuceString.toWideCharPointer() };
	}

	inline juce::String FromUnrealString(const FString& UnrealString)
	{
		return juce::String{ UnrealString.GetCharArray().GetData() };
	}

	template <typename TElement>
	TArray<TElement> ToUnrealArray(const juce::Array<TElement>& JuceArray)
	{
		return TArray<TElement>{ JuceArray.data(), JuceArray.size() };
	}

	template <typename TElement>
	juce::Array<TElement> FromUnrealArray(const TArray<TElement>& UnrealArray)
	{
		return juce::Array<TElement>{ UnrealArray.GetData(), UnrealArray.Num() };
	}

	inline void FillJuceBuffer(juce::AudioBuffer<float>& JuceBuffer, const Audio::FAlignedFloatBuffer* UnrealBuffer, int NumChannels)
	{
		if (!UnrealBuffer)
		{
			return;
		}

		const int NumSamples = UnrealBuffer->Num();
		const float* RawUnrealBuffer = UnrealBuffer->GetData();

		for (int Index = 0; Index < NumSamples; Index += NumChannels)
		{
			for (int Channel = 0; Channel < NumChannels; ++Channel)
			{
				const int SampleIndex = (Index / NumChannels);

				JuceBuffer.setSample(Channel, SampleIndex, RawUnrealBuffer[Index + Channel]);
			}
		}
	}

	inline void FillUnrealAudioBuffer(Audio::FAlignedFloatBuffer* UnrealBuffer, const juce::AudioBuffer<float>& JuceBuffer)
	{
		if (!UnrealBuffer)
		{
			return;
		}

		float* RawUnrealBuffer = UnrealBuffer->GetData();

		const int NumSamples = JuceBuffer.getNumSamples();
		const int NumChannels = JuceBuffer.getNumChannels();

		for (int SampleIndex = 0; SampleIndex < NumSamples; ++SampleIndex)
		{
			for (int Channel = 0; Channel < NumChannels; ++Channel)
			{
				const int InterleavedBaseIndex = SampleIndex * NumChannels;
				RawUnrealBuffer[InterleavedBaseIndex + Channel] = JuceBuffer.getSample(Channel, SampleIndex);
			}
		}
	}
}
