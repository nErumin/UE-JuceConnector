#pragma once
#include "CoreMinimal.h"
#include "JuceHostedPluginAsset.generated.h"

UCLASS(BlueprintType)
class JUCECONNECTOR_API UJuceHostedPluginAsset : public UObject
{
	GENERATED_BODY()
public:
	virtual void Serialize(FArchive& Ar) override;

	UFUNCTION(BlueprintCallable, Category = "Test")
	void TestSomething();
private:
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	FFilePath PluginPath;

	UPROPERTY()
	TArray<uint8> StateMemory;
};
