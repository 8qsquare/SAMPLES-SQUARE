#pragma once
#include <juce_core/juce_core.h>

namespace SampleSquare {
    inline const juce::String APP_NAME = "Sample Square";
    inline const juce::String APP_VERSION = "0.1.0";

    // Colores
    inline const juce::Colour BG_DARK       = juce::Colour(0xff1a1a1f);
    inline const juce::Colour BG_PANEL      = juce::Colour(0xff25252c);
    inline const juce::Colour ACCENT        = juce::Colour(0xff4fc3f7);
    inline const juce::Colour TEXT_PRIMARY  = juce::Colour(0xffe8e8ec);
    inline const juce::Colour TEXT_DIM      = juce::Colour(0xff909099);
    inline const juce::Colour BORDER        = juce::Colour(0xff3a3a44);

    // Layout
    inline constexpr int THUMB_SIZE    = 160;
    inline constexpr int LABEL_HEIGHT  = 24;
    inline constexpr int PADDING       = 12;
    inline constexpr int COLUMNS       = 4;
    inline constexpr int CORNER_R      = 8;
}
