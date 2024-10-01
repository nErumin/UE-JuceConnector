#pragma once
#include "CoreMinimal.h"
#include "JucePluginScanSettings.generated.h"

UCLASS(Config = "EditorPerProjectUserSettings")
class JUCECONNECTOR_API UJucePluginScanSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UJucePluginScanSettings* GetDefaultSettings();
public:
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
public:
	TArray<FString> GetVst3Directories() const;
private:
	UPROPERTY(Config, EditAnywhere, Category = "Plugins", DisplayName = "Vst3 Directories", meta = (AllowPrivateAccess = "true"))
	TArray<FDirectoryPath> Vst3Directories;
};
