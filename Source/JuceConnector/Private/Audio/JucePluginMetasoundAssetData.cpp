#include "Audio/JucePluginMetasoundAssetData.h"

#include "MetasoundDataTypeRegistrationMacro.h"

namespace JuceConnectorMetasound
{
	FJucePluginAssetProxy::FJucePluginAssetProxy(const UJucePluginAsset* PluginAsset)
		: EffectProcessor{ MakeShared<FJucePluginEffectProcessor>() }
	{
		AsyncTask(ENamedThreads::GameThread, [Processor = EffectProcessor, PluginAsset]
		{
			Processor->PrepareProcess(PluginAsset);
		});
	}

	FJucePluginEffectProcessor& FJucePluginAssetProxy::GetProcessor()
	{
		return EffectProcessor.Get();
	}

	FJucePluginAssetData::FJucePluginAssetData(const TSharedPtr<Audio::IProxyData>& InInitData)
		: AssetProxy{ StaticCastSharedPtr<FJucePluginAssetProxy>(InInitData) }
	{
	}

	TSharedPtr<FJucePluginAssetProxy> FJucePluginAssetData::GetProxy() const
	{
		return AssetProxy;
	}

	bool FJucePluginAssetData::IsValid() const
	{
		return AssetProxy.IsValid();
	}

	const FJucePluginAssetProxy* FJucePluginAssetData::operator->() const
	{
		return AssetProxy.Get();
	}

	FJucePluginAssetProxy* FJucePluginAssetData::operator->()
	{
		return AssetProxy.Get();
	}
}

DEFINE_METASOUND_DATA_TYPE(JuceConnectorMetasound::FJucePluginAssetData, "JucePluginAssetData");
