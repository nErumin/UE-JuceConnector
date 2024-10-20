#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FJucePluginSelected, const FString& /* SelectedPluginPath */)

class SJucePluginSelector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SJucePluginSelector) {}
		SLATE_ARGUMENT(TArray<FString>, ScanDirectories)
		SLATE_EVENT(FJucePluginSelected, OnPluginSelected)
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs);
private:
	FText GetSelectedPluginText() const;
	TSharedRef<SWidget> GetComboButtonMenuWidget();
	void ConstructPluginEntries();

	FReply OnRefreshButtonClicked();
	void OnPluginEntrySelected(FText SelectedEntryText);
private:
	TArray<FString> ScanDirectories;
	FJucePluginSelected OnPluginSelected;

	TArray<FString> CachedPluginEntries;
	FText SelectedPluginPath;
};
