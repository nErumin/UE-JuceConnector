#include "Assets/JuceHostedPluginAsset.h"

#include "Assets/JuceHostedPluginAssetVersion.h"

FName UJuceHostedPluginAsset::GetPluginPathPropertyName()
{
	return GET_MEMBER_NAME_CHECKED(UJuceHostedPluginAsset, PluginPath);
}

void UJuceHostedPluginAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FJuceHostedPluginAssetVersion::GUID);

	if (Ar.IsLoading() && Ar.CustomVer(FJuceHostedPluginAssetVersion::GUID) >= FJuceHostedPluginAssetVersion::AddStateSerializationSupport)
	{
		Ar << SerializedStateBlock;
	}
	else if (const TSharedPtr<FJucePluginProxy> AliveProxy = GetPluginProxy().Pin();
			 Ar.IsSaving() && AliveProxy)
	{
		TArray<uint8> StateBlock = AliveProxy->GetState();

		Ar << StateBlock;
	}
}

void UJuceHostedPluginAsset::BeginDestroy()
{
	Super::BeginDestroy();

	if (PluginProxy)
	{
		PluginProxy->OnPluginParameterChanged().RemoveAll(this);
	}
}

FPrimaryAssetId UJuceHostedPluginAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId{ TEXT("JuceHostedPluginAsset"), GetFName() };
}

TWeakPtr<FJucePluginProxy> UJuceHostedPluginAsset::GetPluginProxy() const
{
	if (!PluginProxy && !PluginPath.IsEmpty())
	{
		PluginProxy = MakeShared<FJucePluginProxy>(GetPluginPath());
		RecoverPluginStateFromBlock();

		// Register the callback later, since it causes unexpected event handling like marking dirty...
		PluginProxy->OnPluginParameterChanged().AddUObject(this, &UJuceHostedPluginAsset::OnPluginParameterChanged);
	}

	return PluginProxy;
}

FString UJuceHostedPluginAsset::GetPluginPath() const
{
	// Convert to absolute path.
	const FString GameContentDirectoryPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

	if (FPaths::IsRelative(PluginPath))
	{
		return FPaths::ConvertRelativePathToFull(GameContentDirectoryPath, PluginPath);
	}

	return PluginPath;
}

void UJuceHostedPluginAsset::SetPluginPath(const FString& NewPluginPath)
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

void UJuceHostedPluginAsset::RecoverPluginStateFromBlock() const
{
	if (!SerializedStateBlock.IsEmpty() && PluginProxy)
	{
		PluginProxy->SetState(SerializedStateBlock);
	}
}

void UJuceHostedPluginAsset::OnPluginParameterChanged([[maybe_unused]] const FString& ParameterName, [[maybe_unused]] float NewValue) const
{
#if WITH_EDITOR
	MarkPackageDirty();
#endif
}
