#include "JucePluginSelector.h"

#include "JuceConnectorLogCategory.h"
#include "Juce/Internal/JucePluginLoader.h"
#include "Style/JuceConnectorStyle.h"

#define LOCTEXT_NAMESPACE "SJucePluginSelector"

void SJucePluginSelector::Construct(const FArguments& InArgs)
{
	ScanDirectories = InArgs._ScanDirectories;
	OnPluginSelected = InArgs._OnPluginSelected;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(3.0f, 0.0f)
		[
			SNew(SComboButton)
				.OnGetMenuContent(this, &SJucePluginSelector::GetComboButtonMenuWidget)
				.ButtonContent()
			[
				SNew(STextBlock)
					.Text(this, &SJucePluginSelector::GetSelectedPluginText)
					.Justification(ETextJustify::Left)
			]
		]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(3.0f, 0.0f)
		[
			SNew(SButton)
				.OnClicked(this, &SJucePluginSelector::OnRefreshButtonClicked)
				.Content()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(LOCTEXT("RefreshLabel", "Refresh"))
								.Justification(ETextJustify::Center)
						]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(4.0f, 0.0f, 0.0f, 0.0f)
						[
							SNew(SImage)
								.Image(FJuceConnectorStyle::Get().GetBrush(FJuceConnectorStyle::RefreshIconName))
								.ColorAndOpacity(FSlateColor::UseForeground())
						]
				]
		]
	];

	ConstructPluginEntries();
}

FText SJucePluginSelector::GetSelectedPluginText() const
{
	return SelectedPluginPath.IsEmpty() ?
		LOCTEXT("EmptyComboText", "(Not Selected)") : SelectedPluginPath;
}

TSharedRef<SWidget> SJucePluginSelector::GetComboButtonMenuWidget()
{
	static TSharedRef<FUICommandList> CommandList = MakeShared<FUICommandList>();

	FMenuBuilder Builder{ true, nullptr };
	Builder.AddSearchWidget();
	
	for (const FString& PluginEntry : CachedPluginEntries)
	{
		Builder.AddMenuEntry
		(
			FText::FromString(PluginEntry),
			FText::GetEmpty(),
			FSlateIcon{},
			FUIAction
			{
				FExecuteAction::CreateSP(this, &SJucePluginSelector::OnPluginEntrySelected, FText::FromString(PluginEntry))
			}
		);
	}

	return Builder.MakeWidget();
}

void SJucePluginSelector::ConstructPluginEntries()
{
	CachedPluginEntries = FJucePluginLoader::Get().FindPlugins(ScanDirectories);
}

FReply SJucePluginSelector::OnRefreshButtonClicked()
{
	ConstructPluginEntries();

	return FReply::Handled();
}

void SJucePluginSelector::OnPluginEntrySelected(FText SelectedEntryText)
{
	SelectedPluginPath = SelectedEntryText;

	[[maybe_unused]] bool bExecuteSuccessful = OnPluginSelected.ExecuteIfBound(SelectedEntryText.ToString());
}

#undef LOCTEXT_NAMESPACE
