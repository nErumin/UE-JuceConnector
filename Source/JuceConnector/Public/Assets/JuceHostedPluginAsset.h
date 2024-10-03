#pragma once
#include "CoreMinimal.h"
#include "JuceHostedPluginAsset.generated.h"

class FJucePluginProxy;

UCLASS(BlueprintType)
class JUCECONNECTOR_API UJuceHostedPluginAsset : public UObject
{
	GENERATED_BODY()
public:
	FString GetPluginPath() const;
	void SetPluginPath(const FString& NewPluginPath);
private:
	UPROPERTY(VisibleAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FString PluginPath;
};
