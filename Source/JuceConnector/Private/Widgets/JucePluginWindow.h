#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"

class IJuceProcessorEditorHandle;
class FJucePluginProxy;

class SJucePluginWindow : public SWindow
{
	SLATE_BEGIN_ARGS(SJucePluginWindow) {}
		SLATE_ARGUMENT(FText, Title)
		SLATE_ARGUMENT(TWeakPtr<FJucePluginProxy>, PluginProxy)
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs);

	FIntVector2 GetDefaultEditorPosition() const;
	bool IsAlreadyPluginEditorCreated() const;

	void AddWindowAndAttachPlugin();
	void AttachPluginEditorManually();
private:
	void OnPluginWindowClosed(const TSharedRef<SWindow>& ThisWindow);
private:
	TSharedPtr<IJuceProcessorEditorHandle> EditorHandle;
};
