#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>

namespace SampleSquare {
    namespace Synth {
        class CustomSamplerSound : public juce::SynthesiserSound {
        public:
            // Constructor RAM
            explicit CustomSamplerSound(juce::AudioBuffer<float>&& buffer)
                : ramBuffer(std::move(buffer)), isStreaming(false) {}

            // Constructor Streaming
            explicit CustomSamplerSound(std::unique_ptr<juce::MemoryMappedAudioFormatReader> reader)
                : mappedReader(std::move(reader)), isStreaming(true) {}

            bool appliesToNote(int midiNoteNumber) const override {
                return midiNoteNumber >= lowNote && midiNoteNumber <= highNote;
            }

            bool appliesToChannel(int midiChannel) const override {
                return true;
            }

            // Método solicitado: getSample(channel, pos)
            float getSample(int channel, int pos) const {
                if (isStreaming && mappedReader) {
                    if (channel < 0 || channel >= mappedReader->numChannels || pos < 0 || pos >= mappedReader->lengthInSamples)
                        return 0.0f;
                    juce::AudioBuffer<float> sample(mappedReader->numChannels, 1);
                    mappedReader->read(&sample, 0, 1, pos, true, true);
                    return sample.getSample(juce::jmin(channel, mappedReader->numChannels - 1), 0);
                } else if (!isStreaming && ramBuffer.getNumChannels() > 0) {
                    if (pos < 0 || pos >= ramBuffer.getNumSamples() || channel < 0)
                        return 0.0f;
                    return ramBuffer.getSample(juce::jmin(channel, ramBuffer.getNumChannels() - 1), pos);
                }
                return 0.0f;
            }

            juce::int64 getNumSamples() const noexcept {
                return isStreaming && mappedReader ? mappedReader->lengthInSamples
                                                    : static_cast<juce::int64>(ramBuffer.getNumSamples());
            }

            void readSamples(juce::AudioBuffer<float>& destination, int destinationStart,
                             int numSamples, juce::int64 sourceStart) const {
                const int available = static_cast<int>(juce::jmax<juce::int64>(
                    0, juce::jmin<juce::int64>(static_cast<juce::int64>(numSamples), getNumSamples() - sourceStart)));
                if (available <= 0)
                    return;

                if (isStreaming && mappedReader) {
                    mappedReader->read(&destination, destinationStart, available, sourceStart, true, true);
                    return;
                }

                for (int channel = 0; channel < destination.getNumChannels(); ++channel) {
                    const int sourceChannel = juce::jmin(channel, ramBuffer.getNumChannels() - 1);
                    destination.copyFrom(channel, destinationStart, ramBuffer, sourceChannel,
                                         static_cast<int>(sourceStart), available);
                }
            }

            int lowNote  = 0;
            int highNote = 127;
            int rootNote = 60;

        private:
            juce::AudioBuffer<float> ramBuffer;
            std::unique_ptr<juce::MemoryMappedAudioFormatReader> mappedReader;
            bool isStreaming;
        };
    }
}
