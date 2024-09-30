#include "Juce/JuceMacros.h"

/* This suppresses the name conflict with the Unreal macros. */
#undef verify
#undef ensure

JUCE_INCLUDES_START
#include "juce_graphics/juce_graphics_Harfbuzz.cpp"
JUCE_INCLUDES_END
