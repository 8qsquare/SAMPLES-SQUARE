#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Synth/SharedParams.h"
#include "../Branding.h"

namespace SampleSquare {
    namespace UI {
        class PlayerPanelComponent : public juce::Component, public juce::Slider::Listener {
        public:
            explicit PlayerPanelComponent(Synth::SharedParams& params)
                : sharedParams(params) {
                
                attackSlider.setName("Attack");
                decaySlider.setName("Decay");
                sustainSlider.setName("Sustain");
                releaseSlider.setName("Release");
                gainSlider.setName("Gain");

                auto setupSlider = [&](juce::Slider& s, float min, float max, float val) {
                    s.setRange(min, max, 0.001f);
                    s.setValue(val);
                    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
                    s.addListener(this);
                    addAndMakeVisible(s);
                };

                setupSlider(attackSlider,  0.0f, 2.0f,  sharedParams.attack.load());
                setupSlider(decaySlider,   0.0f, 2.0f,  sharedParams.decay.load());
                setupSlider(sustainSlider, 0.0f, 1.0f,  sharedParams.sustain.load());
                setupSlider(releaseSlider, 0.0f, 5.0f,  sharedParams.release.load());
                setupSlider(gainSlider,    0.0f, 2.0f,  sharedParams.gain.load());
            }

            void resized() override {
                const int sliderWidth = 60;
                const int sliderHeight = 120;
                const int spacing = (getWidth() - 5 * sliderWidth) / 6;

                attackSlider.setBounds (spacing, Branding::PADDING, sliderWidth, sliderHeight);
                decaySlider.setBounds  (spacing * 2 + sliderWidth, Branding::PADDING, sliderWidth, sliderHeight);
                sustainSlider.setBounds(spacing * 3 + sliderWidth * 2, Branding::PADDING, sliderWidth, sliderHeight);
                releaseSlider.setBounds(spacing * 4 + sliderWidth * 3, Branding::PADDING, sliderWidth, sliderHeight);
                gainSlider.setBounds   (spacing * 5 + sliderWidth * 4, Branding::PADDING, sliderWidth, sliderHeight);
            }

            void sliderValueChanged(juce::Slider* slider) override {
                if (slider == &attackSlider)  sharedParams.attack.store(static_cast<float>(slider->getValue()));
                if (slider == &decaySlider)   sharedParams.decay.store(static_cast<float>(slider->getValue()));
                if (slider == &sustainSlider) sharedParams.sustain.store(static_cast<float>(slider->getValue()));
                if (slider == &releaseSlider) sharedParams.release.store(static_cast<float>(slider->getValue()));
                if (slider == &gainSlider)    sharedParams.gain.store(static_cast<float>(slider->getValue()));
            }

        private:
            Synth::SharedParams& sharedParams;
            juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, gainSlider;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerPanelComponent)
        };
    }
}
