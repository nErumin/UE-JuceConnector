#pragma once

class JUCECONNECTOR_API FJucePluginAssetVersion
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
	FJucePluginAssetVersion() = delete;
};
