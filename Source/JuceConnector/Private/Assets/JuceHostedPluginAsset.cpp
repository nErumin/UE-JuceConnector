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

TWeakPtr<FJucePluginProxy> UJuceHostedPluginAsset::GetPluginProxy() const
{
	if (!PluginProxy && !PluginPath.IsEmpty())
	{
		PluginProxy = MakeShared<FJucePluginProxy>(GetPluginPath());

		if (!SerializedStateBlock.IsEmpty())
		{
			PluginProxy->SetState(SerializedStateBlock);
		}
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
