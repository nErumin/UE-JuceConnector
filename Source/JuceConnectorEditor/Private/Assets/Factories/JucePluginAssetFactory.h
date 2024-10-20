#pragma once
#include "CoreMinimal.h"
#include "JucePluginAssetFactory.generated.h"

UCLASS()
class UJucePluginAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UJucePluginAssetFactory();
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
