#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Branding.h"

namespace SampleSquare {
    namespace UI {
        class SampleSquareLookAndFeel : public juce::LookAndFeel_V4 {
        public:
            void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                      const juce::Colour& backgroundColour,
                                      bool isMouseOverButton, bool isButtonDown) override {
                juce::Colour baseColour = backgroundColour.withMultipliedAlpha(button.hasKeyboardFocus(false) ? 1.3f : 1.0f);
                
                if (isButtonDown || isMouseOverButton)
                    baseColour = baseColour.contrasting(0.1f);

                const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat().reduced(1.0f);
                g.setColour(baseColour);
                g.fillRoundedRectangle(bounds, static_cast<float>(Branding::CORNER_R));
                
                g.setColour(Branding::BORDER);
                g.drawRoundedRectangle(bounds, static_cast<float>(Branding::CORNER_R), 1.0f);
            }

            void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                  float sliderPos, float minSliderPos, float maxSliderPos,
                                  juce::Slider::SliderStyle, juce::Slider&) override {
                const juce::Rectangle<int> track(x, y + height / 2 - 2, width, 4);
                g.setColour(Branding::BG_PANEL);
                g.fillRoundedRectangle(track.toFloat(), 2.0f);

                const float pos = juce::jmap(sliderPos, minSliderPos, maxSliderPos, 0.0f, static_cast<float>(width));
                const juce::Rectangle<float> thumb(pos - 8.0f, y + height / 2 - 8.0f, 16.0f, 16.0f);
                
                g.setColour(Branding::ACCENT);
                g.fillEllipse(thumb);
            }

            void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                  float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider&) override {
                const float radius = juce::jmin(width / 2.0f, height / 2.0f) - 4.0f;
                const juce::Point<float> centre(x + width / 2.0f, y + height / 2.0f);
                
                g.setColour(Branding::BG_PANEL);
                g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

                const float angle = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
                
                juce::Path path;
                path.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, rotaryStartAngle, angle);
                
                g.setColour(Branding::ACCENT);
                g.strokePath(path, juce::PathStrokeType(4.0f));
            }
        };
    }
}
