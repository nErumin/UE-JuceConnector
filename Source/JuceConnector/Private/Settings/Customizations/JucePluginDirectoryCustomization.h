#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class FJucePluginDirectoryCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
private:
	bool IsBrowseEnabled(TSharedRef<IPropertyHandle> PathHandle) const;

	FReply OnBrowseButtonClicked(TSharedRef<IPropertyHandle> PathHandle);
private:
	TSharedPtr<SButton> BrowseButton{ nullptr };
};
