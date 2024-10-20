#include "Juce/JuceMacros.h"

/* This suppresses the name conflict with the Unreal macros. */
#undef verify
#undef ensure

JUCE_INCLUDES_START
#include "juce_graphics/juce_graphics_Harfbuzz.cpp"
JUCE_INCLUDES_END

/* Restore the Unreal macro definitions. Their definition are from Misc/AssertionMacros.h. */
#if DO_ENSURE && !USING_CODE_ANALYSIS
#define ensure(InExpression) UE_ENSURE_IMPL (false, InExpression)
#endif

#if DO_CHECK
#define verify(expr) UE_CHECK_IMPL(expr)
#endif
