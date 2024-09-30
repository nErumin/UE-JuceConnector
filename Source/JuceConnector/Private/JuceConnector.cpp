#include "JuceConnector.h"
#include "JuceConnectorStyle.h"
#include "JuceConnectorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName JuceConnectorTabName("JuceConnector");

#define LOCTEXT_NAMESPACE "FJuceConnectorModule"
void FJuceConnectorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FJuceConnectorStyle::Initialize();
	FJuceConnectorStyle::ReloadTextures();

	FJuceConnectorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FJuceConnectorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FJuceConnectorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FJuceConnectorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(JuceConnectorTabName, FOnSpawnTab::CreateRaw(this, &FJuceConnectorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FJuceConnectorTabTitle", "JuceConnector"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FJuceConnectorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FJuceConnectorStyle::Shutdown();

	FJuceConnectorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(JuceConnectorTabName);
}

TSharedRef<SDockTab> FJuceConnectorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FJuceConnectorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("JuceConnector.cpp")));

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FJuceConnectorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(JuceConnectorTabName);
}

void FJuceConnectorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FJuceConnectorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FJuceConnectorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJuceConnectorModule, JuceConnector)
