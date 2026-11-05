#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <functional>
#include <memory>
#include "SharedParams.h"
#include "CustomSamplerVoice.h"
#include "CustomSamplerSound.h"
#include "../UI/PlayerPanelComponent.h"
#include "../Preset/PresetLoader.h"

namespace SampleSquare {
    namespace Synth {
        class CustomSamplerProcessor : public juce::AudioProcessor {
        public:
            CustomSamplerProcessor()
                : AudioProcessor(juce::AudioProcessor::BusesProperties()
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
                  sharedParams(),
                  formatManager(),
                  midiKeyboardState(),
                  playerPanel(std::make_unique<UI::PlayerPanelComponent>(sharedParams)) {
                
                formatManager.registerBasicFormats();
                
                for (int i = 0; i < 16; ++i) {
                    synth.addVoice(new CustomSamplerVoice(sharedParams));
                }
            }

            ~CustomSamplerProcessor() override = default;

            const juce::String getName() const override { return "SampleSquare Processor"; }
            bool hasEditor() const override { return false; }
            juce::AudioProcessorEditor* createEditor() override { return nullptr; }
            bool acceptsMidi() const override { return true; }
            bool producesMidi() const override { return false; }
            bool isMidiEffect() const override { return false; }
            double getTailLengthSeconds() const override { return 0.0; }
            int getNumPrograms() override { return 1; }
            int getCurrentProgram() override { return 0; }
            void setCurrentProgram(int) override {}
            const juce::String getProgramName(int) override { return "Default"; }
            void changeProgramName(int, const juce::String&) override {}

            void prepareToPlay(double sampleRate, int samplesPerBlock) override {
                synth.setCurrentPlaybackSampleRate(sampleRate);
                adsrSampleRate = sampleRate;
            }

            void releaseResources() override {}
            void getStateInformation(juce::MemoryBlock&) override {}
            void setStateInformation(const void*, int) override {}

            void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
                juce::ScopedNoDenormals noDenormals;
                midiKeyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
                auto totalNumInputChannels  = getTotalNumInputChannels();
                auto totalNumOutputChannels = getTotalNumOutputChannels();

                for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
                    buffer.clear(i, 0, buffer.getNumSamples());

                synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
            }

            void loadLibraryAsync(const juce::File& file, std::function<void()> onLibraryLoaded) {
                Preset::PresetLoader::loadAsync(file, [this, onLibraryLoaded](std::vector<std::unique_ptr<CustomSamplerSound>> sounds) {
                    for (auto& s : sounds) {
                        synth.addSound(juce::SynthesiserSound::Ptr(s.release()));
                    }
                    
                    if (onLibraryLoaded) {
                        juce::MessageManager::callAsync(onLibraryLoaded);
                    }
                });
            }

            std::unique_ptr<UI::PlayerPanelComponent> playerPanel;
            SharedParams sharedParams;
            juce::AudioFormatManager formatManager;
            juce::MidiKeyboardState midiKeyboardState;
            juce::Synthesiser synth;
            double adsrSampleRate = 44100.0;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomSamplerProcessor)
        };
    }
}
