#pragma once
#include "CoreMinimal.h"
#include "JucePluginScanSettings.generated.h"

USTRUCT()
struct FJucePluginDirectory
{
	GENERATED_BODY()
public:
	static FName GetDirectoryPathPropertyName();
public:
	FString GetDirectoryPath() const;
private:
	UPROPERTY(EditAnywhere, Category = "Path", meta = (AllowPrivateAccess = "true"))
	FString DirectoryPath;
};

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
	TArray<FJucePluginDirectory> Vst3Directories;
};
