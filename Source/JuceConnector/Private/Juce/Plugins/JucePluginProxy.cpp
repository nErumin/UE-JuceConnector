#include "Juce/Plugins/JucePluginProxy.h"
#include "Juce/Plugins/JuceProcessorEditorHandle.h"

#include "Juce/Internal/JucePluginLoader.h"
#include "Juce/JuceHeader.h"
#include "Juce/Utils/JuceConverters.h"

struct FJucePluginProxy::FImpl
{
public:
	explicit FImpl(const FString& InPluginPath)
		: PluginPath{ InPluginPath }
	{
	}

	~FImpl()
	{
		// Explicitly destroy the editor first. This forces the destruction to be in the right order.
		RequestEditorDestroy();
	}

	FImpl(const FImpl& Other) = delete;
	FImpl(FImpl&& Other) noexcept = default;
	FImpl& operator=(FImpl&& Other) noexcept = default;
	FImpl& operator=(const FImpl& Other) = delete;
public:
	juce::AudioPluginInstance* GetPluginInstance() const
	{
		if (!PluginInstance)
		{
			PluginInstance = FJucePluginLoader::Get().LoadPlugin(PluginPath);
		}

		return PluginInstance.Get();
	}

	TArray<FString> GetParameterNames() const
	{
		EnsureParameterCaches();

		TArray<FString> Names;
		NameToParameterMap.GetKeys(Names);

		return Names;
	}

	TOptional<float> GetParameterValue(const FString& ParameterName) const
	{
		EnsureParameterCaches();

		if (NameToParameterMap.Contains(ParameterName))
		{
			return NameToParameterMap[ParameterName]->getValue();
		}

		return NullOpt;
	}

	TOptional<FText> GetDisplayValueText(const FString& ParameterName) const
	{
		EnsureParameterCaches();

		if (NameToParameterMap.Contains(ParameterName))
		{
			const FString ValueString = JuceConverters::ToUnrealString(NameToParameterMap[ParameterName]->getCurrentValueAsText());
			return FText::FromString(ValueString);
		}

		return NullOpt;
	}

	void SetParameterValue(const FString& ParameterName, float NewValue)
	{
		EnsureParameterCaches();

		if (NameToParameterMap.Contains(ParameterName))
		{
			NameToParameterMap[ParameterName]->setValue(NewValue);
		}
	}

	TWeakPtr<juce::AudioProcessorEditor> GetWeakProcessorEditor() const
	{
		return ProcessorEditor;
	}

	void RequestEditorCreation()
	{
		if (!ProcessorEditor)
		{
			if (juce::AudioPluginInstance* Instance = GetPluginInstance())
			{
				ProcessorEditor = MakeShareable(Instance->createEditorIfNeeded());
			}
		}
	}

	void RequestEditorDestroy()
	{
		if (ProcessorEditor)
		{
			ProcessorEditor.Reset();
		}
	}
private:
	TArray<juce::AudioProcessorParameter*> GetParameters() const
	{
		if (const juce::AudioPluginInstance* Instance = GetPluginInstance())
		{
			return JuceConverters::ToUnrealArray(Instance->getParameters());
		}

		return TArray<juce::AudioProcessorParameter*>{};
	}

	void EnsureParameterCaches() const
	{
		if (NameToParameterMap.IsEmpty())
		{
			NameToParameterMap = ConstructParameterCaches();
		}
	}

	TMap<FString, juce::AudioProcessorParameter*> ConstructParameterCaches() const
	{
		TMap<FString, juce::AudioProcessorParameter*> Cache;

		for (juce::AudioProcessorParameter* Parameter : GetParameters())
		{
			const FString ParameterName{ JuceConverters::ToUnrealString(Parameter->getName(1024)) };

			Cache.Add(ParameterName, Parameter);
		}

		return Cache;
	}
private:
	FString PluginPath;

	TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor;
	mutable TUniquePtr<juce::AudioPluginInstance> PluginInstance{ nullptr };

	mutable TMap<FString, juce::AudioProcessorParameter*> NameToParameterMap;
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

TSharedRef<IJuceProcessorEditorHandle> FJucePluginProxy::CreateEditorHandle()
{
	class FJuceProcessorEditorHandle : public IJuceProcessorEditorHandle
	{
	public:
		explicit FJuceProcessorEditorHandle(const TWeakPtr<FJucePluginProxy>& InWeakProxy)
			: WeakProxy{ InWeakProxy }
		{
		}
	public:
		virtual void Initialize() override
		{
			if (FImpl* Impl = GetProxyImpl())
			{
				Impl->RequestEditorCreation();
			}
		}

		virtual void Finalize() override
		{
			if (FImpl* Impl = GetProxyImpl())
			{
				Impl->RequestEditorDestroy();
			}
		}

		virtual bool IsValid() override
		{
			return GetAudioProcessorEditor().IsValid();
		}

		virtual void AttachToWindow(const TSharedPtr<FGenericWindow>& Window) override
		{
			const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor();

			if (ProcessorEditor && Window)
			{
				ProcessorEditor->setOpaque(true);
				ProcessorEditor->setVisible(true);
				ProcessorEditor->addToDesktop(0, Window->GetOSWindowHandle());
			}
		}

		virtual void DetachFromWindow() override
		{
			if (const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor())
			{
				ProcessorEditor->removeFromDesktop();
			}
		}

		virtual bool IsAttached() const override
		{
			if (const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor())
			{
				return ProcessorEditor->isOnDesktop();
			}

			return false;
		}

		virtual FIntVector2 GetSize() const override
		{
			if (const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor())
			{
				return FIntVector2{ ProcessorEditor->getWidth(), ProcessorEditor->getHeight() };
			}

			return FIntVector2::ZeroValue;
		}

		virtual FIntVector2 GetPosition(const FIntVector2& NewPosition) const override
		{
			if (const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor())
			{
				return FIntVector2{ ProcessorEditor->getX(), ProcessorEditor->getY() };
			}

			return FIntVector2::ZeroValue;
		}

		virtual void SetPosition(const FIntVector2& NewPosition) override
		{
			if (const TSharedPtr<juce::AudioProcessorEditor> ProcessorEditor = GetAudioProcessorEditor())
			{
				ProcessorEditor->setTopLeftPosition(NewPosition.X, NewPosition.Y);
			}
		}
	private:
		FImpl* GetProxyImpl() const
		{
			if (const TSharedPtr<FJucePluginProxy> Proxy = WeakProxy.Pin())
			{
				return Proxy->Impl;
			}

			return nullptr;
		}

		TSharedPtr<juce::AudioProcessorEditor> GetAudioProcessorEditor() const
		{
			if (FImpl* Impl = GetProxyImpl())
			{
				return Impl->GetWeakProcessorEditor().Pin();
			}

			return nullptr;
		}
	private:
		TWeakPtr<FJucePluginProxy> WeakProxy;
	};

	return MakeShared<FJuceProcessorEditorHandle>(AsWeak());
}

TArray<uint8> FJucePluginProxy::GetState() const
{
	if (juce::AudioPluginInstance* Instance = Impl->GetPluginInstance())
	{
		juce::MemoryBlock JuceBlock;
		Instance->getStateInformation(JuceBlock);

		return TArray{ static_cast<uint8*>(JuceBlock.getData()), static_cast<int32>(JuceBlock.getSize()) };
	}

	return TArray<uint8>{};
}

void FJucePluginProxy::SetState(const TArray<uint8>& StateMemoryBlock)
{
	if (juce::AudioPluginInstance* Instance = Impl->GetPluginInstance())
	{
		Instance->setStateInformation(StateMemoryBlock.GetData(), StateMemoryBlock.Num());
	}
}
