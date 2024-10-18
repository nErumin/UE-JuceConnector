#pragma once
#include "CoreMinimal.h"
#include "Juce/Plugins/JucePluginProxy.h"
#include "JuceHostedPluginAsset.generated.h"

class FJucePluginProxy;

UCLASS(BlueprintType, EditInlineNew)
class JUCECONNECTOR_API UJuceHostedPluginAsset : public UObject, public IAudioProxyDataFactory
{
	GENERATED_BODY()
public:
	static FName GetPluginPathPropertyName();
public:
	virtual void Serialize(FArchive& Ar) override;
	virtual void BeginDestroy() override;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	virtual TSharedPtr<Audio::IProxyData> CreateProxyData(const Audio::FProxyDataInitParams& InitParams) override;
public:
	TWeakPtr<FJucePluginProxy> GetPluginProxy() const;

	FString GetPluginPath() const;
	void SetPluginPath(const FString& NewPluginPath);
private:
	void RecoverPluginStateFromBlock() const;

	void OnPluginParameterChanged(const FString& ParameterName, float NewValue) const;
private:
	UPROPERTY(VisibleAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FString PluginPath;

	TArray<uint8> SerializedStateBlock;
	mutable TSharedPtr<FJucePluginProxy> PluginProxy;
};
