#pragma once
#include <juce_core/juce_core.h>

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
