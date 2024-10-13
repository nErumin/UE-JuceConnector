#include "Juce/Plugins/JucePluginProxy.h"
#include "Juce/Plugins/JuceProcessorEditorHandle.h"

#include "Juce/Internal/JucePluginLoader.h"
#include "Juce/JuceHeader.h"
#include "Juce/Utils/JuceConverters.h"

namespace JucePluginProxyInternals
{
	class FManagedPluginInstance final : public juce::AudioProcessorListener
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
			ensureMsgf(PluginInstance, TEXT("The provided instance should be valid when created!"));

			// Cache parameters.
			for (juce::AudioProcessorParameter* Parameter : PluginInstance->getParameters())
			{
				NameToParameterMap.Add(JuceConverters::ToUnrealString(Parameter->getName(1024)), Parameter);
			}
		}

		virtual ~FManagedPluginInstance() override
		{
			DetachFromParent();
		}
	public:
		virtual void audioProcessorParameterChanged(juce::AudioProcessor* processor, int parameterIndex, float newValue) override
		{
			UE_LOG(LogTemp, Error, TEXT("Test: Index - %d, Value - %f"), parameterIndex, newValue);
			PluginInstance->getParameters()[parameterIndex]->setValue(newValue);
		}

		virtual void audioProcessorChanged(juce::AudioProcessor* processor, const ChangeDetails& details) override
		{
			/* Nothing to do */
		}
	public:
		TSharedRef<juce::AudioPluginInstance> GetInternalPlugin() const
		{
			return PluginInstance.ToSharedRef();
		}

		const TMap<FString, juce::AudioProcessorParameter*>& GetNameParameterMap() const
		{
			return NameToParameterMap;
		}

		void AttachToParent(const TWeakPtr<FManagedPluginInstance>& InWeakParent)
		{
			if (const TSharedPtr<FManagedPluginInstance> AliveParent = InWeakParent.Pin())
			{
				DetachFromParent();

				AliveParent->PluginInstance->addListener(this);
				WeakParent = AliveParent;
			}
		}

		void DetachFromParent()
		{
			if (const TSharedPtr<FManagedPluginInstance> AliveParent = WeakParent.Pin())
			{
				AliveParent->PluginInstance->removeListener(this);
			}

			WeakParent.Reset();
		}
	private:
		TSharedPtr<juce::AudioPluginInstance> PluginInstance{ nullptr };
		TMap<FString, juce::AudioProcessorParameter*> NameToParameterMap;

		TWeakPtr<FManagedPluginInstance> WeakParent;
	};

	class FJuceProcessorEditorHandle final : public IJuceProcessorEditorHandle
	{
	public:
		explicit FJuceProcessorEditorHandle(const TWeakPtr<juce::AudioPluginInstance>& InWeakPlugin)
			: WeakPlugin{ InWeakPlugin }
		{
		}

		virtual ~FJuceProcessorEditorHandle()
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
				NameParameterMap[ParameterName]->setValue(NewValue);
			}
		}
	}

	TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> CreateChildInstance() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			return JucePluginProxyInternals::FManagedPluginInstance::Create(PluginPath, Root);
		}

		return nullptr;
	}

	TArray<uint8> GetPluginState() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			juce::MemoryBlock JuceBlock;
			Root->GetInternalPlugin()->getStateInformation(JuceBlock);

			return TArray{ static_cast<uint8*>(JuceBlock.getData()), static_cast<int32>(JuceBlock.getSize()) };
		}

		return TArray<uint8>{};
	}

	void SetPluginState(const TArray<uint8>& StateMemoryBlock)
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			Root->GetInternalPlugin()->setStateInformation(StateMemoryBlock.GetData(), StateMemoryBlock.Num());
		}
	}

	TSharedRef<IJuceProcessorEditorHandle> GetProcessorEditorHandle() const
	{
		if (const TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> Root = GetRootInstance())
		{
			return MakeShared<JucePluginProxyInternals::FJuceProcessorEditorHandle>(Root->GetInternalPlugin());
		}

		return MakeShared<JucePluginProxyInternals::FJuceProcessorEditorHandle>(nullptr);
	}
private:
	TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> GetRootInstance() const
	{
		if (!RootInstance)
		{
			RootInstance = JucePluginProxyInternals::FManagedPluginInstance::Create(PluginPath);
		}

		return RootInstance;
	}
private:
	FString PluginPath;

	mutable TSharedPtr<juce::AudioProcessorEditor> RootInstanceEditor;
	mutable TSharedPtr<JucePluginProxyInternals::FManagedPluginInstance> RootInstance{ nullptr };
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

TSharedRef<IJuceProcessorEditorHandle> FJucePluginProxy::GetEditorHandle() const
{
	return Impl->GetProcessorEditorHandle();
}

TArray<uint8> FJucePluginProxy::GetState() const
{
	return Impl->GetPluginState();
}

void FJucePluginProxy::SetState(const TArray<uint8>& StateMemoryBlock)
{
	Impl->SetPluginState(StateMemoryBlock);
}
