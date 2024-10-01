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
}
