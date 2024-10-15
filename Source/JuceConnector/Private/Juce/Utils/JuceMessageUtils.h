#pragma once

#include "CoreMinimal.h"
#include "Juce/JuceHeader.h"

namespace JuceMessageUtils
{
	template <typename TValue>
	TFuture<TValue> GetCompletedFuture(TValue Value)
	{
		TPromise<TValue> Promise;
		Promise.SetValue(MoveTemp(Value));

		return Promise.GetFuture();
	}

	inline TFuture<void> GetCompletedFuture()
	{
		TPromise<void> Promise;
		Promise.SetValue();

		return Promise.GetFuture();
	}

	template <typename TCallable, typename... TArgs>
	TFuture<TInvokeResult_T<TCallable, TArgs...>> ExecuteOnMessageThreadAsync(TCallable Callable, TArgs&&... Args)
		requires !std::is_same_v<TInvokeResult_T<TCallable, TArgs...>, void>
	{
		using TResult = TInvokeResult_T<TCallable, TArgs...>;
		TSharedRef<TPromise<TResult>> Promise = MakeShared<TPromise<TResult>>();

		if (juce::MessageManager::existsAndIsCurrentThread())
		{
			Promise->SetValue(Invoke(Callable, Forward<TArgs>(Args)...));
		}
		else
		{
			// This point can make some copies.
			juce::MessageManager::callAsync([Promise, Callable = MoveTemp(Callable), Args...]() mutable
			{
				Promise->SetValue(Invoke(Callable, Args...));
			});
		}

		return Promise->GetFuture();
	}

	template <typename TCallable, typename... TArgs>
	TFuture<void> ExecuteOnMessageThreadAsync(TCallable Callable, TArgs&&... Args)
		requires std::is_same_v<TInvokeResult_T<TCallable, TArgs...>, void>
	{
		TSharedRef<TPromise<void>> Promise = MakeShared<TPromise<void>>();

		if (juce::MessageManager::existsAndIsCurrentThread())
		{
			Invoke(Callable, Forward<TArgs>(Args)...);
			Promise->SetValue();
		}
		else
		{
			// This point can make some copies.
			juce::MessageManager::callAsync([Promise, Callable = MoveTemp(Callable), Args...]() mutable
			{
				Invoke(Callable, Args...);
				Promise->SetValue();
			});
		}

		return Promise->GetFuture();
	}
};
