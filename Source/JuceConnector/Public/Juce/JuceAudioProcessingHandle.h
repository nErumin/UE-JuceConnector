#pragma once
#include "CoreMinimal.h"

class IJuceAudioProcessingHandle
{
public:
	IJuceAudioProcessingHandle() = default;
	virtual ~IJuceAudioProcessingHandle() = default;

	IJuceAudioProcessingHandle(const IJuceAudioProcessingHandle& Other) = default;
	IJuceAudioProcessingHandle(IJuceAudioProcessingHandle&& Other) noexcept = default;
protected:
	IJuceAudioProcessingHandle& operator=(const IJuceAudioProcessingHandle& Other) = default;
	IJuceAudioProcessingHandle& operator=(IJuceAudioProcessingHandle&& Other) noexcept = default;
public:
	virtual void Prepare(float SampleRate, int NumChannels, int MaxExpectedSamplesPerBlock) = 0;
	virtual bool IsPrepared() const = 0;

	virtual void ProcessBlock(const TArrayView<const float>& InputBuffer, const TArrayView<float>& OutputBuffer) = 0;
	virtual void Reset() = 0;
};
