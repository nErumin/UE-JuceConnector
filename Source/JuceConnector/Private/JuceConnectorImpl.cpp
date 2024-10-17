#include "JuceConnectorImpl.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Assets/JuceHostedPluginAsset.h"
#include "Assets/Customizations/JucePluginAssetCustomization.h"
#include "Assets/TypeActions/JuceHostedPluginAssetActions.h"
#include "Settings/JucePluginScanSettings.h"
#include "Settings/Customizations/JucePluginDirectoryCustomization.h"
#include "Style/JuceConnectorStyle.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "Audio/JucePluginMetasoundNode.h"

#include "Juce/JuceHeader.h"

#define LOCTEXT_NAMESPACE "FJuceConnectorModule"

void FJuceConnectorModule::StartupModule()
{
	// Use the game thread as message thread.
	juce::MessageManager::getInstance();

	FJuceConnectorStyle::Initialize();

	RegisterAssetTypeActions();
	RegisterTypeCustomizations();
	RegisterAudioProcessTypes();
}

void FJuceConnectorModule::ShutdownModule()
{
	FJuceConnectorStyle::Shutdown();

	UnregisterAssetTypeActions();
	UnregisterTypeCustomizations();
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

void FJuceConnectorModule::RegisterTypeCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout
	(
		FJucePluginDirectory::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FJucePluginDirectoryCustomization::MakeInstance)
	);

	PropertyModule.RegisterCustomClassLayout
	(
		UJuceHostedPluginAsset::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FJucePluginAssetCustomization::MakeInstance)
	);
}

void FJuceConnectorModule::UnregisterTypeCustomizations()
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout(FJucePluginDirectory::StaticStruct()->GetFName());
		PropertyModule->UnregisterCustomClassLayout(UJuceHostedPluginAsset::StaticClass()->GetFName());
	}
}

void FJuceConnectorModule::RegisterAudioProcessTypes()
{
	FMetasoundFrontendRegistryContainer::Get()->EnqueueInitCommand([]
	{
		Metasound::RegisterNodeWithFrontend<JucePluginMetasound::FProcessorNode>();
	});

	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FJuceConnectorModule, JuceConnector)
