#include "JuceConnectorImpl.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Assets/TypeActions/JuceHostedPluginAssetActions.h"
#include "Settings/JucePluginScanSettings.h"
#include "Settings/Customizations/JucePluginDirectoryCustomization.h"
#include "Style/JuceConnectorStyle.h"

#define LOCTEXT_NAMESPACE "FJuceConnectorModule"

void FJuceConnectorModule::StartupModule()
{
	FJuceConnectorStyle::Initialize();

	RegisterAssetTypeActions();
	RegisterPropertyTypeCustomizations();
}

void FJuceConnectorModule::ShutdownModule()
{
	FJuceConnectorStyle::Shutdown();

	UnregisterAssetTypeActions();
	UnregisterPropertyTypeCustomizations();
}

void FJuceConnectorModule::RegisterAssetTypeActions()
{
	const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	const EAssetTypeCategories::Type NewAdvancedCategory =
		AssetTools.RegisterAdvancedAssetCategory(FName{ "JuceConnector" }, LOCTEXT("AssetCategory", "JuceConnector"));

	TArray<TSharedRef<IAssetTypeActions>> Actions
	{
		MakeShared<FJuceHostedPluginAssetActions>(NewAdvancedCategory)
	};

	for (TSharedRef<IAssetTypeActions>& Action : Actions)
	{
		AssetTools.RegisterAssetTypeActions(Action);
	}

	RegisteredAssetTypeActions.Append(MoveTemp(Actions));
}

void FJuceConnectorModule::UnregisterAssetTypeActions()
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

void FJuceConnectorModule::RegisterPropertyTypeCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout
	(
		FJucePluginDirectory::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FJucePluginDirectoryCustomization::MakeInstance)
	);
}

void FJuceConnectorModule::UnregisterPropertyTypeCustomizations()
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout(FJucePluginDirectory::StaticStruct()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FJuceConnectorModule, JuceConnector)
