#include "Settings/Customizations/JucePluginDirectoryCustomization.h"

#include "DesktopPlatformModule.h"
#include "DetailWidgetRow.h"
#include "EditorDirectories.h"
#include "IDesktopPlatform.h"
#include "Settings/JucePluginScanSettings.h"

#define LOCTEXT_NAMESPACE "FJucePluginDirectoryCustomization"

// This customization has almost identical codes with FDirectoryPath given in the engine, but handles a relative path little differently.
TSharedRef<IPropertyTypeCustomization> FJucePluginDirectoryCustomization::MakeInstance()
{
	return MakeShared<FJucePluginDirectoryCustomization>();
}

void FJucePluginDirectoryCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const TSharedPtr<IPropertyHandle> PathHandle = PropertyHandle->GetChildHandle(FJucePluginDirectory::GetDirectoryPathPropertyName());
	check(PathHandle);

	HeaderRow.NameContent()
	[
		PathHandle->CreatePropertyNameWidget(LOCTEXT("PathName", "Directory"))
	]
	.ValueContent()
		.MinDesiredWidth(125.0f)
	    .MaxDesiredWidth(600.0f)
    [
    	SNew(SHorizontalBox)
    	+ SHorizontalBox::Slot()
    		.FillWidth(1.0f)
    		.VAlign(VAlign_Center)
    	[
    		PathHandle->CreatePropertyValueWidget()
    	]
    	+ SHorizontalBox::Slot()
    		.AutoWidth()
    		.Padding(FMargin{ 4.0f, 0.0f, 0.0f, 0.0f })
    		.VAlign(VAlign_Center)
    	[
    		SAssignNew(BrowseButton, SButton)
				.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
				.OnClicked(this, &FJucePluginDirectoryCustomization::OnBrowseButtonClicked, PathHandle.ToSharedRef())
				.ContentPadding(2.0f)
				.ForegroundColor(FSlateColor::UseForeground())
				.IsFocusable(false)
				.IsEnabled(this, &FJucePluginDirectoryCustomization::IsBrowseEnabled, PathHandle.ToSharedRef())
			[
				SNew(SImage)
					.Image(FAppStyle::GetBrush("PropertyWindow.Button_Ellipsis"))
					.ColorAndOpacity(FSlateColor::UseForeground())
			]
    	]
    ];
}

void FJucePluginDirectoryCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	/* Nothing to do */
}

bool FJucePluginDirectoryCustomization::IsBrowseEnabled(TSharedRef<IPropertyHandle> PathHandle) const
{
	return PathHandle->IsEditable();
}

FReply FJucePluginDirectoryCustomization::OnBrowseButtonClicked(TSharedRef<IPropertyHandle> PathHandle)
{
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(BrowseButton.ToSharedRef());
		const void* ParentWindowHandle = nullptr;

		if (const TSharedPtr<FGenericWindow> NativeWindow = (ParentWindow ? ParentWindow->GetNativeWindow() : nullptr); NativeWindow)
		{
			ParentWindowHandle = NativeWindow->GetOSWindowHandle();
		}

		const FString StartDirectory = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT);
		FString PickedDirectory;

		if (DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle,
		                                         LOCTEXT("DirectoryDialogTitle", "Choose a directory").ToString(),
		                                         StartDirectory, PickedDirectory))
		{
			// Convert to relative path, which is rooted to the game content directory.
			const FString GameContentDirectoryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

			if (!FPaths::IsRelative(PickedDirectory))
			{
				FPaths::MakePathRelativeTo(PickedDirectory, *GameContentDirectoryPath);
			}

			PathHandle->SetValue(PickedDirectory);
			FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_IMPORT, PickedDirectory);
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
