#include "AudioPluginConnector.h"
#include "AudioPluginConnectorStyle.h"
#include "AudioPluginConnectorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName AudioPluginConnectorTabName("AudioPluginConnector");

#define LOCTEXT_NAMESPACE "FAudioPluginConnectorModule"

void FAudioPluginConnectorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FAudioPluginConnectorStyle::Initialize();
	FAudioPluginConnectorStyle::ReloadTextures();

	FAudioPluginConnectorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAudioPluginConnectorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FAudioPluginConnectorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAudioPluginConnectorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AudioPluginConnectorTabName, FOnSpawnTab::CreateRaw(this, &FAudioPluginConnectorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FAudioPluginConnectorTabTitle", "AudioPluginConnector"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FAudioPluginConnectorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAudioPluginConnectorStyle::Shutdown();

	FAudioPluginConnectorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AudioPluginConnectorTabName);
}

TSharedRef<SDockTab> FAudioPluginConnectorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FAudioPluginConnectorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("AudioPluginConnector.cpp")));

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

void FAudioPluginConnectorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(AudioPluginConnectorTabName);
}

void FAudioPluginConnectorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FAudioPluginConnectorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FAudioPluginConnectorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAudioPluginConnectorModule, AudioPluginConnector)
