#pragma once
#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FJuceHostedPluginAssetActions final : public FAssetTypeActions_Base
{
public:
	FJuceHostedPluginAssetActions();
	explicit FJuceHostedPluginAssetActions(EAssetTypeCategories::Type InCategoryType);
public:
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
public:
	void SetCategoryType(EAssetTypeCategories::Type NewType);
private:
	EAssetTypeCategories::Type CategoryType;
};
