#pragma once
#include "CoreMinimal.h"
#include "JuceHostedPluginAsset.generated.h"

UCLASS()
class JUCECONNECTOR_API UJuceHostedPluginAsset : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FFilePath PluginPath;
};
