#pragma once

class FJuceHostedPluginAssetVersion
{
public:
	enum Type
	{
		BeforeCustomVersionWasAdded = 0,
		AddStateSerializationSupport = 1,

		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	static const FGuid GUID;
private:
	FJuceHostedPluginAssetVersion() = delete;
};
