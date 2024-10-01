#include "Assets/Factories/JuceHostedPluginAssetFactory.h"

#include "Assets/JuceHostedPluginAsset.h"
#include "Settings/JucePluginScanSettings.h"
#include "Widgets/JucePluginSelector.h"

#define LOCTEXT_NAMESPACE "UJuceHostedPluginAssetFactory"

UJuceHostedPluginAssetFactory::UJuceHostedPluginAssetFactory()
{
	SupportedClass = UJuceHostedPluginAsset::StaticClass();

	bCreateNew = true;
	bEditAfterNew = false;
}

bool UJuceHostedPluginAssetFactory::ConfigureProperties()
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
				.OnPluginSelected_UObject(this, &UJuceHostedPluginAssetFactory::OnTargetPluginSelected)
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
					.OnClicked_UObject(this, &UJuceHostedPluginAssetFactory::OnDecisionButtonClicked, false)
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(6.0f, 0.0f)
			[
				SNew(SButton)
					.Text(LOCTEXT("CancelLabel", "Cancel"))
					.OnClicked_UObject(this, &UJuceHostedPluginAssetFactory::OnDecisionButtonClicked, true)
			]
		]
	];

	GEditor->EditorAddModalWindow(PluginPickerWindow.ToSharedRef());
	PostConfigureProperties();

	return !TargetPluginPath.IsEmpty();
}

UObject* UJuceHostedPluginAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (TargetPluginPath.IsEmpty())
	{
		return nullptr;
	}

	UJuceHostedPluginAsset* Asset = NewObject<UJuceHostedPluginAsset>(InParent, InClass, InName, Flags);
	check(Asset);

	Asset->SetPluginPath(TargetPluginPath);
	return Asset;
}

void UJuceHostedPluginAssetFactory::PreConfigureProperties()
{
	PluginPickerWindow.Reset();
	TargetPluginPath.Empty();
}

void UJuceHostedPluginAssetFactory::PostConfigureProperties()
{
	PluginPickerWindow.Reset();
}

void UJuceHostedPluginAssetFactory::OnTargetPluginSelected(const FString& SelectedPluginPath)
{
	TargetPluginPath = SelectedPluginPath;
}

FReply UJuceHostedPluginAssetFactory::OnDecisionButtonClicked(bool bCancelRequested)
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
