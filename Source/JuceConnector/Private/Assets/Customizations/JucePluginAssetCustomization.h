#pragma once
#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FJucePluginProxy;

class FJucePluginAssetCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
public:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:
	void CustomizePathProperty(IDetailLayoutBuilder& DetailBuilder);
	void CustomizePluginParameterCategory(IDetailLayoutBuilder& DetailBuilder);

	bool IsParameterSliderEnabled(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const;
	float GetParameterSliderValue(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const;
	void OnParameterSliderValueChanged(float NewValue, FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies);

	FText GetParameterValueText(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const;
private:
	FReply OnOpenButtonClicked(TArray<TWeakObjectPtr<UObject>> WeakObjects);
};
