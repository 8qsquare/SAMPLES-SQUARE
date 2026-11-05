#pragma once
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>

namespace SampleSquare {
    namespace UI {
        class PlaceholderArt {
        public:
            static juce::Image generate(const juce::String& name) {
                juce::Image img(juce::Image::ARGB, 256, 256, true);
                juce::Graphics g(img);

                // Gradiente HSV basado en hash
                const int hue = static_cast<int>(static_cast<unsigned int>(name.hashCode()) % 360u);
                
                juce::Colour startCol = juce::Colour::fromHSV(hue / 360.0f, 0.6f, 0.8f, 1.0f);
                juce::Colour endCol   = juce::Colour::fromHSV((hue + 60) / 360.0f, 0.6f, 0.5f, 1.0f);
                
                juce::ColourGradient grad(startCol, 0, 0, endCol, 256, 256, false);
                g.fillGradient(grad, juce::Rectangle<float>(0, 0, 256, 256));

                // Iniciales centradas en bold
                juce::String initials = name.substring(0, 2).toUpperCase();
                juce::Font font(juce::FontOptions(18.0f).withStyle("Bold"));
                g.setColour(juce::Colours::white);
                g.setFont(font);
                g.drawFittedText(initials, 0, 0, 256, 256, juce::Justification::centred, 1);

                return img;
            }
        };
    }
}
