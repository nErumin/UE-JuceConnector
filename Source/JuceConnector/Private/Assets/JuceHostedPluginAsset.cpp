#include "Assets/JuceHostedPluginAsset.h"

#include "JuceConnectorLogCategory.h"
#include "Juce/JuceHeader.h"
#include "Juce/Plugins/JucePluginProxy.h"

void UJuceHostedPluginAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (!Ar.IsLoading())
	{
	}
}

void UJuceHostedPluginAsset::TestSomething()
{
	FJucePluginProxy Proxy{ PluginPath.FilePath };

	for (const FString& Name : Proxy.GetParameterNames())
	{
		UE_LOG(LogJuceConnector, Warning, TEXT("Log: %s"), *Name);
	}
}
