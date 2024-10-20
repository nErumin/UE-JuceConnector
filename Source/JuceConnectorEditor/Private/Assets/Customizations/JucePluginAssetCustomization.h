#pragma once
#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IJucePluginProxy;

class FJucePluginAssetCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
public:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:
	void CustomizePathProperty(IDetailLayoutBuilder& DetailBuilder);
	void CustomizePluginParameterCategory(IDetailLayoutBuilder& DetailBuilder);

	bool IsParameterSliderEnabled(FString ParameterName, TArray<TWeakPtr<IJucePluginProxy>> WeakProxies) const;
	float GetParameterSliderValue(FString ParameterName, TArray<TWeakPtr<IJucePluginProxy>> WeakProxies) const;
	void OnParameterSliderValueChanged(float NewValue, FString ParameterName, TArray<TWeakPtr<IJucePluginProxy>> WeakProxies);

	FText GetParameterValueText(FString ParameterName, TArray<TWeakPtr<IJucePluginProxy>> WeakProxies) const;
private:
	FReply OnOpenButtonClicked(TArray<TWeakObjectPtr<UObject>> WeakObjects);
};
