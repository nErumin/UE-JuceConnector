#include "Assets/Factories/JuceHostedPluginAssetFactory.h"

#include "Assets/JuceHostedPluginAsset.h"

UJuceHostedPluginAssetFactory::UJuceHostedPluginAssetFactory()
{
	SupportedClass = UJuceHostedPluginAsset::StaticClass();

	bCreateNew = true;
	bEditAfterNew = false;
}

UObject* UJuceHostedPluginAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UJuceHostedPluginAsset>(InParent, InClass, InName, Flags);
}
