#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FJuceConnectorStyle
{
public:
	static void Initialize();
	static void Shutdown();

	static FName GetStyleSetName();
	static const ISlateStyle& Get();
private:
	static TUniquePtr<FSlateStyleSet> Create();
	static FString GetStyleResourcePath();
public:
	static FName RefreshIconName;
private:
	static TUniquePtr<FSlateStyleSet> StyleInstance;
};
