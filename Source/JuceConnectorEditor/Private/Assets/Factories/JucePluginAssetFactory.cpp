#include "Assets/Factories/JucePluginAssetFactory.h"

#include "Assets/JucePluginAsset.h"
#include "Settings/JucePluginScanSettings.h"
#include "Widgets/JucePluginSelector.h"

#define LOCTEXT_NAMESPACE "UJucePluginAssetFactory"

UJucePluginAssetFactory::UJucePluginAssetFactory()
{
	SupportedClass = UJucePluginAsset::StaticClass();

	bCreateNew = true;
	bEditAfterNew = false;
}

bool UJucePluginAssetFactory::ConfigureProperties()
{
	PreConfigureProperties();

	const TArray<FString> Vst3Directories = UJucePluginScanSettings::GetDefaultSettings()->GetVst3Directories();
	TArray<FString> ScanDirectories{ Vst3Directories.GetData(), Vst3Directories.Num() };

	SAssignNew(PluginPickerWindow, SWindow)
		.Title(LOCTEXT("Title", "Pick Hosted Plugin"))
		.ClientSize(FVector2D{ 500, 60 })
		.SupportsMinimize(false)
		.SupportsMaximize(false)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
			.AutoHeight()
		[
			SNew(SJucePluginSelector)
				.ScanDirectories(ScanDirectories)
				.OnPluginSelected_UObject(this, &UJucePluginAssetFactory::OnTargetPluginSelected)
		]
		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 5.0f, 0.0f, 1.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
			[
				SNew(SButton)
					.Text(LOCTEXT("OKLabel", "OK"))
					.OnClicked_UObject(this, &UJucePluginAssetFactory::OnDecisionButtonClicked, false)
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(6.0f, 0.0f)
			[
				SNew(SButton)
					.Text(LOCTEXT("CancelLabel", "Cancel"))
					.OnClicked_UObject(this, &UJucePluginAssetFactory::OnDecisionButtonClicked, true)
			]
		]
	];

	GEditor->EditorAddModalWindow(PluginPickerWindow.ToSharedRef());
	PostConfigureProperties();

	return !TargetPluginPath.IsEmpty();
}

UObject* UJucePluginAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (TargetPluginPath.IsEmpty())
	{
		return nullptr;
	}

	UJucePluginAsset* Asset = NewObject<UJucePluginAsset>(InParent, InClass, InName, Flags);
	check(Asset);

	Asset->SetPluginPath(TargetPluginPath);
	return Asset;
}

void UJucePluginAssetFactory::PreConfigureProperties()
{
	PluginPickerWindow.Reset();
	TargetPluginPath.Empty();
}

void UJucePluginAssetFactory::PostConfigureProperties()
{
	PluginPickerWindow.Reset();
}

void UJucePluginAssetFactory::OnTargetPluginSelected(const FString& SelectedPluginPath)
{
	TargetPluginPath = SelectedPluginPath;
}

FReply UJucePluginAssetFactory::OnDecisionButtonClicked(bool bCancelRequested)
{
	if (bCancelRequested)
	{
		TargetPluginPath.Empty();
	}

	if (PluginPickerWindow)
	{
		FSlateApplication::Get().RequestDestroyWindow(PluginPickerWindow.ToSharedRef());
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
