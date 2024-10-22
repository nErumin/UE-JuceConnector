#pragma once

#include "CoreMinimal.h"
#include "MetasoundAudioBuffer.h"
#include "MetasoundBuilderInterface.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundFacade.h"

#include "Audio/JucePluginMetasoundAssetData.h"

#define LOCTEXT_NAMESPACE "FJucePluginMetasoundNode"

namespace JuceConnectorMetasound
{
	using namespace Metasound;

	class FProcessOperator final : public TExecutableOperator<FProcessOperator>
	{
	public:
		struct FInputs
		{
			FAudioBufferReadRef AudioInput;
			FJucePluginAssetDataReadRef PluginAsset;
		};

		struct FOutputs
		{
			FAudioBufferWriteRef AudioOutput;
		};

	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults);

	public:
		explicit FProcessOperator(const FBuildOperatorParams& InParams, FInputs InInputs);
	public:
		virtual void BindInputs(FInputVertexInterfaceData& InVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InVertexData) override;

	public:
		void Execute();
		void Reset(const IOperator::FResetParams& InParams);

	private:
		FInputs Inputs;
		FOutputs Outputs;

		float SampleRate;
	};

	class FProcessorNode final : public FNodeFacade
	{
	public:
		explicit FProcessorNode(const FNodeInitData& InitData);
	};
}

#undef LOCTEXT_NAMESPACE
