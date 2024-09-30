#include "Assets/TypeActions/JuceHostedPluginAssetActions.h"
#include "Assets/JuceHostedPluginAsset.h"

#define LOCTEXT_NAMESPACE "FJuceHostedPluginAssetActions"

FJuceHostedPluginAssetActions::FJuceHostedPluginAssetActions()
	: FJuceHostedPluginAssetActions{ EAssetTypeCategories::Misc }
{
}

FJuceHostedPluginAssetActions::FJuceHostedPluginAssetActions(EAssetTypeCategories::Type InCategoryType)
	: CategoryType{ InCategoryType }
{
}

FText FJuceHostedPluginAssetActions::GetName() const
{
	return LOCTEXT("Name", "JuceHostedPlugin");
}

FColor FJuceHostedPluginAssetActions::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FJuceHostedPluginAssetActions::GetSupportedClass() const
{
	return UJuceHostedPluginAsset::StaticClass();
}

uint32 FJuceHostedPluginAssetActions::GetCategories()
{
	return CategoryType;
}

void FJuceHostedPluginAssetActions::SetCategoryType(EAssetTypeCategories::Type NewType)
{
	CategoryType = NewType;
}

#undef LOCTEXT_NAMESPACE
