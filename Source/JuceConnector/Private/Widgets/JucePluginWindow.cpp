#include "JucePluginWindow.h"

#include "Juce/Plugins/JucePluginProxy.h"
#include "Juce/Plugins/JuceProcessorEditorHandle.h"

#define LOCTEXT_NAMESPACE "SJucePluginWindow"

void SJucePluginWindow::Construct(const FArguments& InArgs)
{
	if (const TSharedPtr<FJucePluginProxy> AliveProxy = InArgs._PluginProxy.Pin())
	{
		EditorHandle = AliveProxy->GetEditorHandle();
	}

	SWindow::Construct
	(
		SWindow::FArguments{}
		.Title(InArgs._Title)
		.ClientSize(FVector2D{ 650, 650 })
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		.SizingRule(ESizingRule::FixedSize)
	);

	WindowClosedEvent.AddSP(this, &SJucePluginWindow::OnPluginWindowClosed);
}

bool SJucePluginWindow::IsAlreadyPluginEditorCreated() const
{
	return !EditorHandle->IsAttachable();
}

void SJucePluginWindow::AttachPluginEditorManually()
{
	if (!IsAlreadyPluginEditorCreated())
	{
		EditorHandle->AttachToWindow(GetNativeWindow());
		EditorHandle->SetPosition(GetDefaultEditorPosition());

		const FVector2f FittedWindowSize(EditorHandle->GetSize().X, EditorHandle->GetSize().Y);
		Resize(FittedWindowSize);
	}
}

FIntVector2 SJucePluginWindow::GetDefaultEditorPosition() const
{
	FIntVector2 EditorPosition{ static_cast<int>(GetWindowBorderSize().Left), 0 };

	if (const FOptionalSize BarSize = GetTitleBarSize(); BarSize.IsSet())
	{
		EditorPosition.Y = static_cast<int>(BarSize.Get()) + 1;
	}

	return EditorPosition;
}

void SJucePluginWindow::AddWindowAndAttachPlugin()
{
	FSlateApplication::Get().AddWindow(StaticCastSharedRef<SWindow>(AsShared()));

	// To display the editor correctly, the attachment happens after adding the window.
	AttachPluginEditorManually();
}

void SJucePluginWindow::OnPluginWindowClosed([[maybe_unused]] const TSharedRef<SWindow>& ThisWindow)
{
	ensure(IsAlreadyPluginEditorCreated());

	EditorHandle->DetachFromWindow();
}

#undef LOCTEXT_NAMESPACE
