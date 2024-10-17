#include "Audio/JucePluginMetasoundNode.h"

#include "MetasoundNodeInterface.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "FJucePluginMetasoundNode"

namespace JucePluginMetasound
{
	namespace Params
	{
		METASOUND_PARAM(InParamAudio, "Input", "Audio input to be processed.");
		METASOUND_PARAM(InParamPluginAsset, "Plugin", "JUCE-hosted plugin asset used for the audio processing.");
		METASOUND_PARAM(OutParamAudio, "Output", "Processed audio output.");
	}

	const FNodeClassMetadata& FProcessOperator::GetNodeInfo()
	{
		using namespace Metasound;

		static FNodeClassMetadata NodeInfo
		{
			.ClassName = FNodeClassName{ TEXT("JuceConnector"), TEXT("Process"), NAME_None },
			.MajorVersion = 1,
			.MinorVersion = 0,
			.DisplayName = METASOUND_LOCTEXT("ProcessDisplayName", "Process (JUCE)"),
			.Description = METASOUND_LOCTEXT("ProcessDescription", "Process an audio input by using a plugin hosted via JUCE."),
			.Author = TEXT("nErumin"),
			.PromptIfMissing = PluginNodeMissingPrompt,
			.DefaultInterface = GetVertexInterface(),
			.CategoryHierarchy = TArray<FText>{ METASOUND_LOCTEXT("Metasound_JuceConnectorCategory", "JuceConnector") }
		};

		return NodeInfo;
	}

	const FVertexInterface& FProcessOperator::GetVertexInterface()
	{
		static FVertexInterface DefaultInterface
		{
			FInputVertexInterface
			{
				TInputDataVertex<FAudioBuffer>{ METASOUND_GET_PARAM_NAME_AND_METADATA(Params::InParamAudio) },
			},
			FOutputVertexInterface
			{
				TOutputDataVertex<FAudioBuffer>{ METASOUND_GET_PARAM_NAME_AND_METADATA(Params::OutParamAudio) }
			}
		};

		return DefaultInterface;
	}

	TUniquePtr<IOperator> FProcessOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
	{
		const FInputVertexInterfaceData& InputData = InParams.InputData;

		FInputs Inputs
		{
			.AudioInput = InputData.GetOrCreateDefaultDataReadReference<FAudioBuffer>(METASOUND_GET_PARAM_NAME(Params::InParamAudio), InParams.OperatorSettings)
		};

		return MakeUnique<FProcessOperator>(InParams, MoveTemp(Inputs));
	}

	FProcessOperator::FProcessOperator(const FBuildOperatorParams& InParams, FInputs InInputs)
		: Inputs{ MoveTemp(InInputs) },
		  Outputs{ .AudioOutput = FAudioBufferWriteRef::CreateNew(InParams.OperatorSettings) }
	{
	}

	void FProcessOperator::BindInputs(FInputVertexInterfaceData& InVertexData)
	{
		InVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(Params::InParamAudio), Inputs.AudioInput);
	}

	void FProcessOperator::BindOutputs(FOutputVertexInterfaceData& InVertexData)
	{
		InVertexData.BindWriteVertex(METASOUND_GET_PARAM_NAME(Params::OutParamAudio), Outputs.AudioOutput);
	}

	void FProcessOperator::Execute()
	{
	}

	void FProcessOperator::Reset(const IOperator::FResetParams& InParams)
	{
	}

	FProcessorNode::FProcessorNode(const FNodeInitData& InitData)
		: FNodeFacade{ InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FProcessOperator>{} }
	{
	}
}

#undef LOCTEXT_NAMESPACE
