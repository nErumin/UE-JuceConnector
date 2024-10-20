#include "JuceConnectorEditorImpl.h"

#include "AssetToolsModule.h"
#include "Assets/JucePluginAsset.h"
#include "Assets/Customizations/JucePluginAssetCustomization.h"
#include "Assets/TypeActions/JucePluginAssetActions.h"
#include "Settings/JucePluginScanSettings.h"
#include "Settings/Customizations/JucePluginDirectoryCustomization.h"
#include "Style/JuceConnectorStyle.h"

#include "MetasoundEditorModule.h"

#define LOCTEXT_NAMESPACE "FJuceConnectorEditorImpl"

void FJuceConnectorEditorImpl::StartupModule()
{
	FJuceConnectorStyle::Initialize();

	RegisterAssetTypeActions();
	RegisterTypeCustomizations();
	RegisterMetasoundPinNodes();
}

void FJuceConnectorEditorImpl::ShutdownModule()
{
	UnregisterTypeCustomizations();
	UnregisterAssetTypeActions();
	
	FJuceConnectorStyle::Shutdown();
}

void FJuceConnectorEditorImpl::RegisterAssetTypeActions()
{
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	const EAssetTypeCategories::Type NewAdvancedCategory =
		AssetTools.RegisterAdvancedAssetCategory(FName{ "JuceConnector" }, LOCTEXT("AssetCategory", "JuceConnector"));

	TArray<TSharedRef<IAssetTypeActions>> Actions
	{
		MakeShared<FJucePluginAssetActions>(NewAdvancedCategory)
	};

	for (TSharedRef<IAssetTypeActions>& Action : Actions)
	{
		AssetTools.RegisterAssetTypeActions(Action);
	}

	RegisteredAssetTypeActions.Append(MoveTemp(Actions));
}

void FJuceConnectorEditorImpl::UnregisterAssetTypeActions()
{
	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		IAssetTools& AssetTools = AssetToolsModule->Get();

		for (TSharedRef<IAssetTypeActions>& Action : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action);
		}
	}

	RegisteredAssetTypeActions.Empty();
}

void FJuceConnectorEditorImpl::RegisterTypeCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout
	(
		FJucePluginDirectory::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FJucePluginDirectoryCustomization::MakeInstance)
	);

	PropertyModule.RegisterCustomClassLayout
	(
		UJucePluginAsset::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FJucePluginAssetCustomization::MakeInstance)
	);
}

void FJuceConnectorEditorImpl::UnregisterTypeCustomizations()
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout(FJucePluginDirectory::StaticStruct()->GetFName());
		PropertyModule->UnregisterCustomClassLayout(UJucePluginAsset::StaticClass()->GetFName());
	}
}

void FJuceConnectorEditorImpl::RegisterMetasoundPinNodes()
{
	using namespace Metasound::Editor;

	IMetasoundEditorModule& MetaSoundEditorModule = FModuleManager::LoadModuleChecked<IMetasoundEditorModule>("MetaSoundEditor");

	const FGraphPinParams PinParams
	{
		.PinCategory = "object",
		.PinSubcategory = NAME_None,
		.PinColor = nullptr,
		.PinConnectedIcon = nullptr,
		.PinDisconnectedIcon = nullptr
	};

	MetaSoundEditorModule.RegisterCustomPinType("JucePluginAssetData", PinParams);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJuceConnectorEditorImpl, JuceConnectorEditor)
