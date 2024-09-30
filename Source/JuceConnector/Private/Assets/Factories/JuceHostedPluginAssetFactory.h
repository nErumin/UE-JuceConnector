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
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
