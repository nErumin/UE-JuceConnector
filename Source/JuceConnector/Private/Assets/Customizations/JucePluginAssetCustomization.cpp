﻿#include "Assets/Customizations/JucePluginAssetCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Assets/JuceHostedPluginAsset.h"
#include "Widgets/JucePluginWindow.h"
#include "Widgets/Input/SSlider.h"

#define LOCTEXT_NAMESPACE "FJucePluginAssetCustomization"

namespace JucePluginAssetCustomizationInternal
{
	TArray<UJuceHostedPluginAsset*> GetAlivePluginAssetObjects(const TArray<TWeakObjectPtr<UObject>>& WeakObjects)
	{
		TArray<UJuceHostedPluginAsset*> AliveObjects;

		for (const TWeakObjectPtr<UObject>& WeakObject : WeakObjects)
		{
			if (UJuceHostedPluginAsset* CastedObject = Cast<UJuceHostedPluginAsset>(WeakObject.Get()))
			{
				AliveObjects.Add(CastedObject);
			}
		}

		return AliveObjects;
	}

	TArray<UJuceHostedPluginAsset*> GetAlivePluginAssetsBeingCustomized(const IDetailLayoutBuilder& DetailBuilder)
	{
		TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
		DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

		return GetAlivePluginAssetObjects(CustomizedObjects);
	}

	bool ContainsMultipleSources(const TArray<UJuceHostedPluginAsset*>& PluginAssets)
	{
		if (PluginAssets.IsEmpty())
		{
			return false;
		}

		const FString UniquePath{ PluginAssets[0]->GetPluginPath() };

		for (int Index = 1; Index < PluginAssets.Num(); ++Index)
		{
			if (!UniquePath.Equals(PluginAssets[Index]->GetPluginPath()))
			{
				return true;
			}
		}

		return false;
	}
}

TSharedRef<IDetailCustomization> FJucePluginAssetCustomization::MakeInstance()
{
	return MakeShared<FJucePluginAssetCustomization>();
}

void FJucePluginAssetCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CustomizePathProperty(DetailBuilder);
	CustomizePluginParameterCategory(DetailBuilder);
}

void FJucePluginAssetCustomization::CustomizePathProperty(IDetailLayoutBuilder& DetailBuilder)
{
	const TSharedRef<IPropertyHandle> PathHandle = DetailBuilder.GetProperty(UJuceHostedPluginAsset::GetPluginPathPropertyName());
	PathHandle->MarkHiddenByCustomization();

	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(PathHandle->GetDefaultCategoryName());
	IDetailPropertyRow& PropertyRow = CategoryBuilder.AddProperty(PathHandle);

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	const FResetToDefaultOverride ResetToDefaultOverride = FResetToDefaultOverride::Create(TAttribute{ false });
	PropertyRow.OverrideResetToDefault(ResetToDefaultOverride);

	PropertyRow.CustomWidget().NameContent()
	[
		PathHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
		[
			PathHandle->CreatePropertyValueWidget()
		]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(3.0f, 0.0f, 0.0f, 0.0f)
		[
			SNew(SButton)
				.Text(LOCTEXT("OpenButtonLabel", "Open"))
				.OnClicked(this, &FJucePluginAssetCustomization::OnOpenButtonClicked, CustomizedObjects)
		]
	];
}

void FJucePluginAssetCustomization::CustomizePluginParameterCategory(IDetailLayoutBuilder& DetailBuilder)
{
	using namespace JucePluginAssetCustomizationInternal;

	const TArray<UJuceHostedPluginAsset*> AliveAssets = GetAlivePluginAssetsBeingCustomized(DetailBuilder);

	if (AliveAssets.IsEmpty() || ContainsMultipleSources(AliveAssets))
	{
		// Cannot customize the parameter section when the plugins are from empty or multiple sources.
		return;
	}

	TArray<TWeakPtr<FJucePluginProxy>> WeakProxies;
	TArray<FString> ParameterNames;

	for (const UJuceHostedPluginAsset* Asset : AliveAssets)
	{
		const TWeakPtr<FJucePluginProxy>& WeakProxy = WeakProxies.Add_GetRef(Asset->GetPluginProxy());

		if (const TSharedPtr<FJucePluginProxy> AliveProxy = WeakProxy.Pin(); ParameterNames.IsEmpty())
		{
			ParameterNames = AliveProxy->GetParameterNames();
		}
	}

	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory("Parameters");
	TSharedPtr<IPropertyHandle> Handle = DetailBuilder.GetProperty(UJuceHostedPluginAsset::GetPluginPathPropertyName());

	for (const FString& ParameterName : ParameterNames)
	{
		const FText ParameterNameText = FText::FromString(ParameterName);

		CategoryBuilder.AddCustomRow(ParameterNameText)
		.NameContent()
		[
			Handle->CreatePropertyNameWidget(ParameterNameText, ParameterNameText)
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
			[
				SNew(SBox)
					.MinDesiredWidth(160.0f)
					.MaxDesiredWidth(300.0f)
				[
					SNew(SSlider)
						.MinValue(0.0f)
						.MaxValue(1.0f)
						.IsEnabled(this, &FJucePluginAssetCustomization::IsParameterSliderEnabled, ParameterName, WeakProxies)
						.Value(this, &FJucePluginAssetCustomization::GetParameterSliderValue, ParameterName, WeakProxies)
						.OnValueChanged(this, &FJucePluginAssetCustomization::OnParameterSliderValueChanged, ParameterName, WeakProxies)
				]
			]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(6.0f, 0.0f)
			[
				SNew(STextBlock)
					.Text(this, &FJucePluginAssetCustomization::GetParameterValueText, ParameterName, WeakProxies)
			]
		];
	}
}

bool FJucePluginAssetCustomization::IsParameterSliderEnabled(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const
{
	if (WeakProxies.IsEmpty())
	{
		return false;
	}

	TOptional<float> UniqueValue;

	for (const TWeakPtr<FJucePluginProxy>& WeakProxy : WeakProxies)
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = WeakProxy.Pin())
		{
			const float Value = AliveProxy->GetNormalizedParameterValue(ParameterName);

			if (UniqueValue && !FMath::IsNearlyEqual(*UniqueValue, Value))
			{
				return false;
			}

			UniqueValue = Value;
		}
	}

	return true;
}

float FJucePluginAssetCustomization::GetParameterSliderValue(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const
{
	float SliderValue = 0.0f;

	for (const TWeakPtr<FJucePluginProxy>& WeakProxy : WeakProxies)
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = WeakProxy.Pin())
		{
			SliderValue = AliveProxy->GetNormalizedParameterValue(ParameterName);
		}
	}

	return SliderValue;
}

void FJucePluginAssetCustomization::OnParameterSliderValueChanged(float NewValue, FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies)
{
	ensure(NewValue >= 0.0f && NewValue <= 1.0f);

	for (const TWeakPtr<FJucePluginProxy>& WeakProxy : WeakProxies)
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = WeakProxy.Pin())
		{
			AliveProxy->SetParameterValue(ParameterName, NewValue);
		}
	}
}

FText FJucePluginAssetCustomization::GetParameterValueText(FString ParameterName, TArray<TWeakPtr<FJucePluginProxy>> WeakProxies) const
{
	TSet<FString> ValueStringSet;

	for (const TWeakPtr<FJucePluginProxy>& WeakProxy : WeakProxies)
	{
		if (const TSharedPtr<FJucePluginProxy> AliveProxy = WeakProxy.Pin())
		{
			ValueStringSet.Add(AliveProxy->GetNormalizedParameterValueAsText(ParameterName).ToString());
		}
	}

	return ValueStringSet.Num() == 1 ?
		FText::FromString(*ValueStringSet.CreateConstIterator()) :
		LOCTEXT("MultipleParameterValue", "Multiple Values");
}

FReply FJucePluginAssetCustomization::OnOpenButtonClicked(TArray<TWeakObjectPtr<UObject>> WeakObjects)
{
	using namespace JucePluginAssetCustomizationInternal;

	for (const UJuceHostedPluginAsset* PluginAsset : GetAlivePluginAssetObjects(WeakObjects))
	{
		TSharedPtr<SJucePluginWindow> JucePluginWindow{ nullptr };

		SAssignNew(JucePluginWindow, SJucePluginWindow)
			.PluginProxy(PluginAsset->GetPluginProxy());

		if (JucePluginWindow->IsAlreadyPluginWindowCreated())
		{
			FMessageDialog::Open(EAppMsgCategory::Error, EAppMsgType::Ok, LOCTEXT("AlreadyOpenedPlugin", "Cannot open more than two windows for one plugin instance."));
		}
		else
		{
			JucePluginWindow->AddWindowAndAttachPlugin();
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
