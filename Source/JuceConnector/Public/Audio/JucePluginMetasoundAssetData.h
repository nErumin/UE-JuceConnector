#pragma once

#include "CoreMinimal.h"
#include "MetasoundDataReferenceMacro.h"
#include "Assets/JucePluginAsset.h"
#include "Audio/JucePluginEffectProcessor.h"

#define LOCTEXT_NAMESPACE "FJucePluginMetasoundNode"

class UJucePluginAsset;

namespace JuceConnectorMetasound
{
	class JUCECONNECTOR_API FJucePluginAssetProxy final : public Audio::TProxyData<FJucePluginAssetProxy>
	{
	public:
		IMPL_AUDIOPROXY_CLASS(FJucePluginAssetProxy)
	public:
		explicit FJucePluginAssetProxy(const UJucePluginAsset* PluginAsset);
	public:
		FJucePluginEffectProcessor& GetProcessor();
	private:
		TSharedRef<FJucePluginEffectProcessor> EffectProcessor;
	};

	class JUCECONNECTOR_API FJucePluginAssetData final
	{
	public:
		FJucePluginAssetData() = default;
		FJucePluginAssetData(const FJucePluginAssetData& Other) = default;
		FJucePluginAssetData(FJucePluginAssetData&& Other) noexcept = default;
		FJucePluginAssetData& operator=(const FJucePluginAssetData& Other) = default;
		FJucePluginAssetData& operator=(FJucePluginAssetData&& Other) noexcept = default;

		explicit FJucePluginAssetData(const TSharedPtr<Audio::IProxyData>& InInitData);
	public:
		TSharedPtr<FJucePluginAssetProxy> GetProxy() const;
		bool IsValid() const;

		FJucePluginAssetProxy* operator->();
		const FJucePluginAssetProxy* operator->() const;

	private:
		TSharedPtr<FJucePluginAssetProxy> AssetProxy;
	};
}

DECLARE_METASOUND_DATA_REFERENCE_TYPES(JuceConnectorMetasound::FJucePluginAssetData, JUCECONNECTOR_API, FJucePluginAssetDataTypeInfo, FJucePluginAssetDataReadRef, FJucePluginAssetDataWriteRef);

#undef LOCTEXT_NAMESPACE
