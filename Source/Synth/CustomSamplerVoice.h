#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "SharedParams.h"
#include "CustomSamplerSound.h"

namespace SampleSquare {
    namespace Synth {
        class CustomSamplerVoice : public juce::SynthesiserVoice {
        public:
            explicit CustomSamplerVoice(SharedParams& params)
                : sharedParams(params), cachedParams(params.snapshot()), sampleCache(2, 32768) {
                adsr.setSampleRate(44100.0);
                updateEnvelopeParameters();
            }

            void setCurrentPlaybackSampleRate(double sampleRate) override {
                juce::SynthesiserVoice::setCurrentPlaybackSampleRate(sampleRate);
                adsr.setSampleRate(sampleRate);
                updateEnvelopeParameters();
            }

            bool canPlaySound(juce::SynthesiserSound* sound) override {
                return dynamic_cast<CustomSamplerSound*>(sound) != nullptr;
            }

            void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override {
                auto* samplerSound = dynamic_cast<CustomSamplerSound*>(sound);
                if (!samplerSound) return;

                const auto snapshot = sharedParams.snapshot();
                if (snapshot.a != cachedParams.a || snapshot.d != cachedParams.d ||
                    snapshot.s != cachedParams.s || snapshot.r != cachedParams.r) {
                    cachedParams = snapshot;
                    updateEnvelopeParameters();
                    adsr.reset();
                }

                adsr.noteOn();
                currentNote = midiNoteNumber;
                rootNote = samplerSound->rootNote;
                samplePos = 0;
                gainValue = cachedParams.g;
                noteVelocity = velocity;
            }

            void stopNote(float velocity, bool allowTailOff) override {
                if (allowTailOff) {
                    adsr.noteOff();
                } else {
                    clearCurrentNote();
                    samplePos = 0;
                }
            }

            void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
                auto* sound = dynamic_cast<CustomSamplerSound*>(getCurrentlyPlayingSound().get());
                if (!sound) return;

                const auto snapshot = sharedParams.snapshot();
                if (snapshot.a != cachedParams.a || snapshot.d != cachedParams.d ||
                    snapshot.s != cachedParams.s || snapshot.r != cachedParams.r ||
                    snapshot.g != cachedParams.g) {
                    cachedParams = snapshot;
                    updateEnvelopeParameters();
                    gainValue = cachedParams.g;
                }

                const int numChannels = outputBuffer.getNumChannels();
                const double pitchRatio = std::pow(2.0, (static_cast<double>(currentNote) - rootNote) / 12.0);
                auto cacheStart = static_cast<juce::int64>(samplePos);
                auto cacheLength = static_cast<int>(juce::jmin<juce::int64>(
                    static_cast<juce::int64>(sampleCache.getNumSamples()), sound->getNumSamples() - cacheStart));
                if (cacheLength > 0)
                    sound->readSamples(sampleCache, 0, cacheLength, cacheStart);

                for (int sample = 0; sample < numSamples; ++sample) {
                    if (samplePos >= sound->getNumSamples()) {
                        clearCurrentNote();
                        break;
                    }

                    const float envelope = adsr.getNextSample();
                    if (envelope <= 0.0f) {
                        clearCurrentNote();
                        break;
                    }

                    const int pos = static_cast<int>(samplePos);
                    const int nextPos = pos + 1;
                    const float alpha = static_cast<float>(samplePos - pos);

                    if (pos < cacheStart || nextPos >= cacheStart + cacheLength) {
                        cacheStart = pos;
                        cacheLength = static_cast<int>(juce::jmin<juce::int64>(
                            static_cast<juce::int64>(sampleCache.getNumSamples()), sound->getNumSamples() - cacheStart));
                        if (cacheLength > 0)
                            sound->readSamples(sampleCache, 0, cacheLength, cacheStart);
                    }

                    for (int channel = 0; channel < numChannels; ++channel) {
                        const auto cachePos = pos - static_cast<int>(cacheStart);
                        const float s1 = cachePos >= 0 && cachePos < cacheLength
                                             ? sampleCache.getSample(juce::jmin(channel, sampleCache.getNumChannels() - 1), cachePos)
                                             : 0.0f;
                        const float s2 = cachePos + 1 >= 0 && cachePos + 1 < cacheLength
                                             ? sampleCache.getSample(juce::jmin(channel, sampleCache.getNumChannels() - 1), cachePos + 1)
                                             : 0.0f;
                        const float interpolated = s1 + alpha * (s2 - s1);
                        
                        outputBuffer.addSample(channel, startSample + sample,
                                               interpolated * envelope * gainValue * noteVelocity);
                    }

                    samplePos += pitchRatio;
                    if (samplePos >= sound->getNumSamples()) {
                        clearCurrentNote();
                        break;
                    }
                }
            }

            void pitchWheelMoved(int newPitchWheelPosition) override {}
            void controllerMoved(int controllerNumber, int newControllerValue) override {}

        private:
            void updateEnvelopeParameters() {
                adsr.setParameters(juce::ADSR::Parameters(cachedParams.a, cachedParams.d,
                                                          cachedParams.s, cachedParams.r));
            }

            SharedParams& sharedParams;
            ParamsSnapshot cachedParams;
            juce::ADSR adsr;
            juce::AudioBuffer<float> sampleCache;
            int currentNote = 0;
            int rootNote = 60;
            double samplePos = 0.0;
            float gainValue = 1.0f;
            float noteVelocity = 1.0f;
        };
    }
}
