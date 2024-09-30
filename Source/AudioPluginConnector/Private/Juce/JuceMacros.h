#pragma once

#include "GenericPlatform/GenericPlatformCompilerPreSetup.h"

#if !defined(__clang__) /* Compiler settings for MSVC */
	#define JUCE_INCLUDES_START \
		THIRD_PARTY_INCLUDES_START \
		__pragma(warning(push)) \
		__pragma(warning(disable: 4018)) \
		__pragma(warning(disable: 4065)) \
		__pragma(warning(disable: 4334)) \
		__pragma(warning(disable: 4146)) \
		__pragma(warning(disable: 4102)) \
		__pragma(warning(disable: 4191)) \
		__pragma(warning(disable: 4777)) \
		__pragma(warning(disable: 5105))

	#define JUCE_INCLUDES_END \
		THIRD_PARTY_INCLUDES_END \
		__pragma(warning(pop))

	#define JUCE_SUPPRESS_CPP_MACROS_START \
		__pragma(warning(push)) \
		__pragma(warning(disable: 4668))

	#define JUCE_SUPPRESS_CPP_MACROS_END \
		__pragma(warning(pop))
#endif
