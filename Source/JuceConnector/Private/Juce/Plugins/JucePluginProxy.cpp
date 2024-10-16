#include "Juce/Plugins/JucePluginProxy.h"

#include "Juce/Plugins/JuceAudioProcessingHandle.h"
#include "Juce/Plugins/JuceProcessorEditorHandle.h"

#include "Juce/Internal/JucePluginLoader.h"
#include "Juce/JuceHeader.h"
#include "Juce/Utils/JuceConverters.h"

namespace JucePluginProxyInternals
{
	FString ExtractParameterName(const juce::AudioProcessorParameter* Parameter)
	{
		return JuceConverters::ToUnrealString(Parameter->getName(1024));
	}

	class FDelegateConnectingProcessorListener final : public juce::AudioProcessorListener
	{
	public:
		virtual void audioProcessorParameterChanged(juce::AudioProcessor* Processor, int ParameterIndex, float NewValue) override
		{
			const juce::AudioProcessorParameter* Parameter = Processor->getParameters()[ParameterIndex];

			if (ParameterChanged.IsBound())
			{
				ParameterChanged.Broadcast(ExtractParameterName(Parameter), NewValue);
			}
		}

		virtual void audioProcessorChanged(juce::AudioProcessor* Processor, const ChangeDetails& Details) override
		{
			/* Nothing to do */
		}
	public:
		FJucePluginParameterChanged ParameterChanged;
	};

	class FManagedPluginInstance final : public TSharedFromThis<FManagedPluginInstance>
	{
	public:
		static TSharedPtr<FManagedPluginInstance> Create(const FString& PluginPath, const TWeakPtr<FManagedPluginInstance>& WeakParent = nullptr)
		{
			if (TUniquePtr<juce::AudioPluginInstance> Instance = FJucePluginLoader::Get().LoadPlugin(PluginPath))
			{
				const TSharedPtr<FManagedPluginInstance> ManagedInstance = MakeShared<FManagedPluginInstance>(MoveTemp(Instance));
				ManagedInstance->AttachToParent(WeakParent);

				return ManagedInstance;
			}

			return nullptr;
		}
	public:
		explicit FManagedPluginInstance(TUniquePtr<juce::AudioPluginInstance> InPluginInstance)
			: PluginInstance{ MakeShareable(InPluginInstance.Release()) }
		{
			// Cache parameters.
			for (juce::AudioProcessorParameter* Parameter : PluginInstance->getParameters())
			{
				NameToParameterMap.Add(ExtractParameterName(Parameter), Parameter);
			}

			PluginInstance->addListener(&ProcessorListener);
		}

		~FManagedPluginInstance()
		{
			DetachFromParent();

			PluginInstance->removeListener(&ProcessorListener);
		}
	private:
		void OnParentParameterChanged(const FString& ParameterName, float NewValue)
		{
			if (NameToParameterMap.Contains(ParameterName))
			{
				NameToParameterMap[ParameterName]->setValue(NewValue);
			}
		}
	public:
		TSharedRef<juce::AudioPluginInstance> GetInternalPlugin() const
		{
			return PluginInstance;
		}

		const TMap<FString, juce::AudioProcessorParameter*>& GetNameParameterMap() const
		{
			return NameToParameterMap;
		}

		juce::MemoryBlock GetStateMemoryBlock() const
		{
			juce::MemoryBlock Block;
			GetInternalPlugin()->getStateInformation(Block);

			return Block;
		}

		void SetStateMemoryBlock(const void* Data, int Size)
		{
			GetInternalPlugin()->setStateInformation(Data, Size);
		}

		FDelegateConnectingProcessorListener& GetProcessorListener()
		{
			return ProcessorListener;
		}

		void AttachToParent(const TWeakPtr<FManagedPluginInstance>& InWeakParent)
		{
			if (const TSharedPtr<FManagedPluginInstance> AliveParent = InWeakParent.Pin())
			{
				DetachFromParent();

				CopyStateFromParent(InWeakParent);
				AliveParent->GetProcessorListener().ParameterChanged.AddSP(this, &FManagedPluginInstance::OnParentParameterChanged);

				WeakParent = AliveParent;
			}
		}

		void DetachFromParent()
		{
			if (const TSharedPtr<FManagedPluginInstance> AliveParent = WeakParent.Pin())
			{
				AliveParent->GetProcessorListener().ParameterChanged.RemoveAll(this);
			}

			WeakParent.Reset();
		}
	private:
		void CopyStateFromParent(const TWeakPtr<FManagedPluginInstance>& InWeakParent)
		{
			if (const TSharedPtr<FManagedPluginInstance> AliveParent = InWeakParent.Pin())
			{
				juce::MemoryBlock ParentStateBlock = AliveParent->GetStateMemoryBlock();

				SetStateMemoryBlock(ParentStateBlock.getData(), ParentStateBlock.getSize());
			}
		}
	private:
		TWeakPtr<FManagedPluginInstance> WeakParent;

		TSharedRef<juce::AudioPluginInstance> PluginInstance;
		TMap<FString, juce::AudioProcessorParameter*> NameToParameterMap;
		FDelegateConnectingProcessorListener ProcessorListener;
	};

	class FJuceProcessorEditorHandle final : public IJuceProcessorEditorHandle
	{
	public:
		explicit FJuceProcessorEditorHandle(const TWeakPtr<juce::AudioPluginInstance>& InWeakPlugin)
			: WeakPlugin{ InWeakPlugin }
		{
		}

		virtual ~FJuceProcessorEditorHandle() override
		{
			DetachFromWindow();
		}
	public:
		virtual bool AttachToWindow(const TSharedPtr<FGenericWindow>& Window) override
		{
			if (!Window || !IsAttachable())
			{
				return false;
			}

			if (const TSharedPtr<juce::AudioPluginInstance> AlivePlugin = WeakPlugin.Pin())
			{
				CreatedEditor = TUniquePtr<juce::AudioProcessorEditor>(AlivePlugin->createEditorIfNeeded());
			}

			if (CreatedEditor)
			{
				CreatedEditor->setOpaque(true);
				CreatedEditor->setVisible(true);
				CreatedEditor->addToDesktop(0, Window->GetOSWindowHandle());

				return true;
			}

			return false;
		}

		virtual bool DetachFromWindow() override
		{
			if (CreatedEditor)
			{
				CreatedEditor->removeFromDesktop();
				CreatedEditor.Reset();

				return true;
			}

			return false;
		}

		virtual bool IsAttachable() const override
		{
			if (const TSharedPtr<juce::AudioPluginInstance> AlivePlugin = WeakPlugin.Pin())
			{
				return !AlivePlugin->getActiveEditor();
			}

			return false;
		}

		virtual FIntVector2 GetSize() const override
		{
			if (CreatedEditor)
			{
				return FIntVector2{ CreatedEditor->getWidth(), CreatedEditor->getHeight() };
			}

			return FIntVector2::ZeroValue;
		}

		virtual FIntVector2 GetPosition(const FIntVector2& NewPosition) const override
		{
			if (CreatedEditor)
			{
				return FIntVector2{ CreatedEditor->getX(), CreatedEditor->getY() };
			}

			return FIntVector2::ZeroValue;
		}

		virtual void SetPosition(const FIntVector2& NewPosition) override
		{
			if (CreatedEditor)
			{
				CreatedEditor->setTopLeftPosition(NewPosition.X, NewPosition.Y);
			}
		}
	private:
		TUniquePtr<juce::AudioProcessorEditor> CreatedEditor{ nullptr };
		TWeakPtr<juce::AudioPluginInstance> WeakPlugin;
	};

	class FJuceAudioProcessingHandle final : public IJuceAudioProcessingHandle
	{
	public:
		explicit FJuceAudioProcessingHandle(const TSharedPtr<FManagedPluginInstance>& InManagedInstance)
			: ManagedInstance{ InManagedInstance }
		{
		}
	public:
		virtual void Prepare(float SampleRate, int NumChannels, int MaxExpectedSamplesPerBlock) override
		{
			if (ManagedInstance)
			{
				ManagedInstance->GetInternalPlugin()->prepareToPlay(SampleRate, MaxExpectedSamplesPerBlock);
				PreparedNumChannels = NumChannels;
			}
		}

		virtual bool IsPrepared() const override
		{
			return PreparedNumChannels.IsSet();
		}

		virtual void ProcessBlock(const Audio::FAlignedFloatBuffer* InputBuffer, Audio::FAlignedFloatBuffer* OutputBuffer) override
		{
			if (ManagedInstance && IsPrepared())
			{
				const int NumSamples = InputBuffer->Num() / *PreparedNumChannels;

				juce::AudioBuffer<float> SampleBuffer{ *PreparedNumChannels, NumSamples };
				juce::MidiBuffer MidiBuffer;

				JuceConverters::FillJuceBuffer(SampleBuffer, InputBuffer, *PreparedNumChannels);
				ManagedInstance->GetInternalPlugin()->processBlock(SampleBuffer, MidiBuffer);

				JuceConverters::FillUnrealAudioBuffer(OutputBuffer, SampleBuffer);
			}
		}

		virtual void ProcessBlock(const TArrayView<float>& InputBuffer, const TArrayView<float>& OutputBuffer) override
		{
			if (ManagedInstance && IsPrepared())
			{
				const int NumSamples = InputBuffer.Num() / *PreparedNumChannels;

				juce::AudioBuffer<float> SampleBuffer{ *PreparedNumChannels, NumSamples };
				juce::MidiBuffer MidiBuffer;

				JuceConverters::FillJuceBuffer(SampleBuffer, InputBuffer, *PreparedNumChannels);
				ManagedInstance->GetInternalPlugin()->processBlock(SampleBuffer, MidiBuffer);

				JuceConverters::FillUnrealAudioBuffer(OutputBuffer, SampleBuffer);
			}
		}

		virtual void Reset() override
		{
			if (ManagedInstance)
			{
				ManagedInstance->GetInternalPlugin()->reset();
				PreparedNumChannels = NullOpt;
			}
		}
	private:
		TSharedPtr<FManagedPluginInstance> ManagedInstance;
		TOptional<int> PreparedNumChannels{ NullOpt };
	};
}

struct FJucePluginProxy::FImpl
{
public:
	explicit FImpl(const FString& InPluginPath)
		: PluginPath{ InPluginPath }
	{
	}

	~FImpl()
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			Root->GetProcessorListener().ParameterChanged.RemoveAll(this);
		}
	}

	FImpl(const FImpl& Other) = delete;
	FImpl(FImpl&& Other) noexcept = default;
	FImpl& operator=(FImpl&& Other) noexcept = default;
	FImpl& operator=(const FImpl& Other) = delete;
public:
	TArray<FString> GetParameterNames() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			TArray<FString> Names;
			Root->GetNameParameterMap().GetKeys(Names);

			return Names;
		}

		return TArray<FString>{};
	}

	TOptional<float> GetParameterValue(const FString& ParameterName) const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			if (const auto& NameParameterMap = Root->GetNameParameterMap(); NameParameterMap.Contains(ParameterName))
			{
				return NameParameterMap[ParameterName]->getValue();
			}
		}

		return NullOpt;
	}

	TOptional<FText> GetDisplayValueText(const FString& ParameterName) const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			if (const auto& NameParameterMap = Root->GetNameParameterMap(); NameParameterMap.Contains(ParameterName))
			{
				const FString DisplayTextString = JuceConverters::ToUnrealString(NameParameterMap[ParameterName]->getCurrentValueAsText());
				return FText::FromString(DisplayTextString);
			}
		}

		return NullOpt;
	}

	void SetParameterValue(const FString& ParameterName, float NewValue)
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			if (const auto& NameParameterMap = Root->GetNameParameterMap(); NameParameterMap.Contains(ParameterName))
			{
				NameParameterMap[ParameterName]->setValueNotifyingHost(NewValue);
			}
		}
	}

	TArray<uint8> GetPluginState() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			juce::MemoryBlock JuceBlock = Root->GetStateMemoryBlock();

			return TArray{ static_cast<uint8*>(JuceBlock.getData()), static_cast<int32>(JuceBlock.getSize()) };
		}

		return TArray<uint8>{};
	}

	void SetPluginState(const TArray<uint8>& StateMemoryBlock)
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			Root->SetStateMemoryBlock(StateMemoryBlock.GetData(), StateMemoryBlock.Num());
		}
	}

	FJucePluginParameterChanged& OnPluginParameterChanged()
	{
		return RootParameterChanged;
	}

	TSharedRef<IJuceProcessorEditorHandle> GetProcessorEditorHandle() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			return MakeShared<JucePluginProxyInternals::FJuceProcessorEditorHandle>(Root->GetInternalPlugin());
		}

		return MakeShared<JucePluginProxyInternals::FJuceProcessorEditorHandle>(nullptr);
	}

	TSharedRef<JucePluginProxyInternals::FJuceAudioProcessingHandle> CreateNewProcessingHandle() const
	{
		using namespace JucePluginProxyInternals;

		if (const TSharedPtr<FManagedPluginInstance> Root = GetRootInstance())
		{
			return MakeShared<FJuceAudioProcessingHandle>(FManagedPluginInstance::Create(PluginPath, Root));
		}

		return MakeShared<FJuceAudioProcessingHandle>(nullptr);
	}
private:
	TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> GetRootInstance() const
	{
		if (!RootInstance)
		{
			RootInstance = JucePluginProxyInternals::FManagedPluginInstance::Create(PluginPath);

			RootInstance->GetProcessorListener().ParameterChanged.AddRaw(this, &FImpl::OnRootParameterChanged);
		}

		return RootInstance;
	}

	void OnRootParameterChanged(const FString& ParameterName, float NewValue) const
	{
		if (IsInGameThread())
		{
			RootParameterChanged.Broadcast(ParameterName, NewValue);
		}
		else
		{
			::AsyncTask(ENamedThreads::GameThread, [this, ParameterName, NewValue]
			{
				RootParameterChanged.Broadcast(ParameterName, NewValue);
			});
		}
	}
private:
	FString PluginPath;

	mutable TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> RootInstance{ nullptr };
	FJucePluginParameterChanged RootParameterChanged;
};

FJucePluginProxy::FJucePluginProxy(const FString& PluginPath)
	: Impl{ new FImpl(PluginPath) }
{
	check(Impl);
}

FJucePluginProxy::~FJucePluginProxy()
{
	const FImpl* ImplAddress = Impl;
	Impl = nullptr;

	delete ImplAddress;
}

TArray<FString> FJucePluginProxy::GetParameterNames() const
{
	return Impl->GetParameterNames();
}

FText FJucePluginProxy::GetParameterValueAsText(const FString& ParameterName) const
{
	if (const TOptional<FText> Value = Impl->GetDisplayValueText(ParameterName); Value)
	{
		return *Value;
	}

	return FText::GetEmpty();
}

float FJucePluginProxy::GetNormalizedParameterValue(const FString& ParameterName, float DefaultValue) const
{
	if (const TOptional<float> Value = Impl->GetParameterValue(ParameterName); Value)
	{
		return *Value;
	}

	return DefaultValue;
}

FText FJucePluginProxy::GetNormalizedParameterValueAsText(const FString& ParameterName) const
{
	if (const TOptional<float> Value = Impl->GetParameterValue(ParameterName); Value)
	{
		return FText::FromString(FString::Printf(TEXT("%f"), *Value));
	}

	return FText::GetEmpty();
}

void FJucePluginProxy::SetParameterValue(const FString& ParameterName, float NewValue)
{
	Impl->SetParameterValue(ParameterName, NewValue);
}

FJucePluginParameterChanged& FJucePluginProxy::OnPluginParameterChanged()
{
	return Impl->OnPluginParameterChanged();
}

TSharedRef<IJuceProcessorEditorHandle> FJucePluginProxy::GetEditorHandle() const
{
	return Impl->GetProcessorEditorHandle();
}

TSharedRef<IJuceAudioProcessingHandle> FJucePluginProxy::CreateNewProcessingHandle() const
{
	return Impl->CreateNewProcessingHandle();
}

TArray<uint8> FJucePluginProxy::GetState() const
{
	return Impl->GetPluginState();
}

void FJucePluginProxy::SetState(const TArray<uint8>& StateMemoryBlock)
{
	Impl->SetPluginState(StateMemoryBlock);
}
