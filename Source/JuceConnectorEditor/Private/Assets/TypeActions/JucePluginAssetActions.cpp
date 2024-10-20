#include "Assets/TypeActions/JucePluginAssetActions.h"
#include "Assets/JucePluginAsset.h"

#define LOCTEXT_NAMESPACE "FJucePluginAssetActions"

FJucePluginAssetActions::FJucePluginAssetActions()
	: FJucePluginAssetActions{ EAssetTypeCategories::Misc }
{
}

FJucePluginAssetActions::FJucePluginAssetActions(EAssetTypeCategories::Type InCategoryType)
	: CategoryType{ InCategoryType }
{
}

FText FJucePluginAssetActions::GetName() const
{
	return LOCTEXT("Name", "JucePlugin");
}

FColor FJucePluginAssetActions::GetTypeColor() const
{
	return FColor::Purple;
}

UClass* FJucePluginAssetActions::GetSupportedClass() const
{
	return UJucePluginAsset::StaticClass();
}

uint32 FJucePluginAssetActions::GetCategories()
{
	return CategoryType;
}

void FJucePluginAssetActions::SetCategoryType(EAssetTypeCategories::Type NewType)
{
	CategoryType = NewType;
}

#undef LOCTEXT_NAMESPACE
