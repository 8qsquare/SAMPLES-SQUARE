#pragma once
#include <juce_graphics/juce_graphics.h>

namespace SampleSquare {
    namespace Library {
        struct LibraryInfo {
            juce::String name;
            juce::File folder;
            juce::File presetFile;
            juce::Image coverImage;
        };
    }
}
