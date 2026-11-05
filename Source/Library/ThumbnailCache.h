#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <unordered_map>
#include "../UI/PlaceholderArt.h"

namespace SampleSquare {
    namespace Library {
        class ThumbnailCache {
        public:
            static ThumbnailCache& getInstance() {
                static ThumbnailCache instance;
                return instance;
            }

            juce::Image getThumbnail(const juce::File& presetFile) {
                const auto cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                        .getChildFile("SampleSquare")
                                        .getChildFile("thumbs");
                cacheDir.createDirectory();

                const auto hashKey = presetFile.getFullPathName() + "_" + juce::String(presetFile.getLastModificationTime().toMilliseconds());
                const auto hash = juce::String::toHexString(static_cast<juce::uint32>(hashKey.hashCode()));
                const auto cacheFile = cacheDir.getChildFile(hash + ".png");

                if (cacheFile.existsAsFile()) {
                    juce::Image img;
                    juce::ImageFileFormat::loadFrom(img, cacheFile);
                    return img;
                }

                // Generar Placeholder si no existe
                juce::Image img = PlaceholderArt::generate(presetFile.getFileNameWithoutExtension());
                juce::PNGImageFormat pngFormat;
                juce::FileOutputStream out(cacheFile);
                pngFormat.writeImageToStream(img, out);
                
                return img;
            }

        private:
            ThumbnailCache() = default;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThumbnailCache)
        };
    }
}
