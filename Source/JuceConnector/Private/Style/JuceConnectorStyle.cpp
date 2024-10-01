#include "Style/JuceConnectorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FJuceConnectorStyle::RefreshIconName{ "FJuceConnectorStyle.RefreshIcon" };

TUniquePtr<FSlateStyleSet> FJuceConnectorStyle::StyleInstance{ nullptr };

const ISlateStyle& FJuceConnectorStyle::Get()
{
	return *StyleInstance;
}

void FJuceConnectorStyle::Initialize()
{
	if (!StyleInstance)
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FJuceConnectorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(Get());

	StyleInstance.Reset();
}

FName FJuceConnectorStyle::GetStyleSetName()
{
	return FName{ "JuceConnectorStyle" };
}

TUniquePtr<FSlateStyleSet> FJuceConnectorStyle::Create()
{
	static const FVector2D Icon20x20{ 20, 20 };

	TUniquePtr<FSlateStyleSet> NewStyleSet = MakeUnique<FSlateStyleSet>(GetStyleSetName());
	NewStyleSet->SetContentRoot(GetStyleResourcePath());

	NewStyleSet->Set(RefreshIconName, new FSlateImageBrush(NewStyleSet->RootToContentDir(TEXT("Refresh"), TEXT(".png")), Icon20x20));
	return MoveTemp(NewStyleSet);
}

FString FJuceConnectorStyle::GetStyleResourcePath()
{
	const TSharedPtr<IPlugin> FoundPlugin = IPluginManager::Get().FindPlugin("JuceConnector");
	check(FoundPlugin);

	return FoundPlugin->GetBaseDir() / TEXT("Resources");
}
