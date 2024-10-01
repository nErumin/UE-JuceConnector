#pragma once
#include "CoreMinimal.h"
#include "JuceHostedPluginAssetFactory.generated.h"

UCLASS()
class UJuceHostedPluginAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UJuceHostedPluginAssetFactory();
public:
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
private:
	void PreConfigureProperties();
	void PostConfigureProperties();

	void OnTargetPluginSelected(const FString& SelectedPluginPath);
	FReply OnDecisionButtonClicked(bool bCancelRequested);
private:
	TSharedPtr<SWindow> PluginPickerWindow{ nullptr };
	FString TargetPluginPath;
};
