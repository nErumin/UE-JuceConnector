#pragma once
#include "CoreMinimal.h"
#include "Juce/Plugins/JucePluginProxy.h"
#include "JuceHostedPluginAsset.generated.h"

class FJucePluginProxy;

UCLASS(BlueprintType)
class JUCECONNECTOR_API UJuceHostedPluginAsset : public UObject
{
	GENERATED_BODY()
public:
	static FName GetPluginPathPropertyName();
public:
	TWeakPtr<FJucePluginProxy> GetPluginProxy() const;

	FString GetPluginPath() const;
	void SetPluginPath(const FString& NewPluginPath);
private:
	UPROPERTY(VisibleAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FString PluginPath;

	mutable TSharedPtr<FJucePluginProxy> PluginProxy;
};
