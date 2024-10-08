#pragma once

#include "CoreMinimal.h"

class IJuceProcessorEditorHandle
{
public:
	IJuceProcessorEditorHandle() = default;
	virtual ~IJuceProcessorEditorHandle() = default;

	IJuceProcessorEditorHandle(const IJuceProcessorEditorHandle& Other) = default;
	IJuceProcessorEditorHandle(IJuceProcessorEditorHandle&& Other) noexcept = default;
protected:
	IJuceProcessorEditorHandle& operator=(const IJuceProcessorEditorHandle& Other) = default;
	IJuceProcessorEditorHandle& operator=(IJuceProcessorEditorHandle&& Other) noexcept = default;
public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual bool IsValid() = 0;

	virtual void AttachToWindow(const TSharedPtr<FGenericWindow>& Window) = 0;
	virtual void DetachFromWindow() = 0;
	virtual bool IsAttached() const = 0;

	virtual FIntVector2 GetSize() const = 0;

	virtual FIntVector2 GetPosition(const FIntVector2& NewPosition) const = 0;
	virtual void SetPosition(const FIntVector2& NewPosition) = 0;
};
