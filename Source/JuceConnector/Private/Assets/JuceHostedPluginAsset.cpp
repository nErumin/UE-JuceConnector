﻿#include "Assets/JuceHostedPluginAsset.h"

FName UJuceHostedPluginAsset::GetPluginPathPropertyName()
{
	return GET_MEMBER_NAME_CHECKED(UJuceHostedPluginAsset, PluginPath);
}

TWeakPtr<FJucePluginProxy> UJuceHostedPluginAsset::GetPluginProxy() const
{
	if (!PluginProxy && !PluginPath.IsEmpty())
	{
		PluginProxy = MakeShared<FJucePluginProxy>(GetPluginPath());
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
