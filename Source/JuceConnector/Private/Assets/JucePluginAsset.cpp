#include "Assets/JucePluginAsset.h"

#include "Assets/JucePluginAssetVersion.h"
#include "Audio/JucePluginMetasoundAssetData.h"
#include "Juce/JucePluginProxyImpl.h"

FName UJucePluginAsset::GetPluginPathPropertyName()
{
	return GET_MEMBER_NAME_CHECKED(UJucePluginAsset, PluginPath);
}

void UJucePluginAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FJucePluginAssetVersion::GUID);

	if (Ar.IsLoading() && Ar.CustomVer(FJucePluginAssetVersion::GUID) >= FJucePluginAssetVersion::AddStateSerializationSupport)
	{
		Ar << SerializedStateBlock;
	}
	else if (const TSharedPtr<IJucePluginProxy> AliveProxy = GetPluginProxy().Pin();
			 Ar.IsSaving() && AliveProxy)
	{
		TArray<uint8> StateBlock = AliveProxy->GetState();

		Ar << StateBlock;
	}
}

void UJucePluginAsset::BeginDestroy()
{
	Super::BeginDestroy();

	if (PluginProxy)
	{
		PluginProxy->OnPluginParameterChanged().RemoveAll(this);
	}
}

FPrimaryAssetId UJucePluginAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId{ TEXT("JucePluginAsset"), GetFName() };
}

TSharedPtr<Audio::IProxyData> UJucePluginAsset::CreateProxyData(const Audio::FProxyDataInitParams& InitParams)
{
	return MakeShared<JuceConnectorMetasound::FJucePluginAssetProxy>(this);
}

TWeakPtr<IJucePluginProxy> UJucePluginAsset::GetPluginProxy() const
{
	if (!PluginProxy && !PluginPath.IsEmpty())
	{
		PluginProxy = MakeShared<FJucePluginProxy>(GetPluginPath());
		RecoverPluginStateFromBlock();

		// Register the callback later, since it causes unexpected event handling like marking dirty...
		PluginProxy->OnPluginParameterChanged().AddUObject(this, &UJucePluginAsset::OnPluginParameterChanged);
	}

	return PluginProxy;
}

FString UJucePluginAsset::GetPluginPath() const
{
	// Convert to absolute path.
	const FString GameContentDirectoryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

	if (FPaths::IsRelative(PluginPath))
	{
		return FPaths::ConvertRelativePathToFull(GameContentDirectoryPath, PluginPath);
	}

	return PluginPath;
}

void UJucePluginAsset::SetPluginPath(const FString& NewPluginPath)
{
	// Convert to relative path, which is rooted to the game content directory.
	const FString GameContentDirectoryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	FString ConvertedPath = NewPluginPath;

	if (!FPaths::IsRelative(ConvertedPath))
	{
		FPaths::MakePathRelativeTo(ConvertedPath, *GameContentDirectoryPath);
	}

	PluginPath = ConvertedPath;
}

void UJucePluginAsset::RecoverPluginStateFromBlock() const
{
	if (!SerializedStateBlock.IsEmpty() && PluginProxy)
	{
		PluginProxy->SetState(SerializedStateBlock);
	}
}

void UJucePluginAsset::OnPluginParameterChanged([[maybe_unused]] const FString& ParameterName, [[maybe_unused]] float NewValue) const
{
#if WITH_EDITOR
	[[maybe_unused]] const bool bDirtySuccess = MarkPackageDirty();
#endif
}
