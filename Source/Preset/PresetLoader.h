#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>
#include <memory>
#include "DspresetParser.h"
#include "../Synth/CustomSamplerSound.h"

namespace SampleSquare {
    namespace Preset {
        class PresetLoader {
        public:
            using SoundVector = std::vector<std::unique_ptr<Synth::CustomSamplerSound>>;

            static void loadAsync(const juce::File& file, std::function<void(SoundVector)> callback) {
                juce::Thread::launch([file, callback]() {
                    auto parsed = DspresetParser::parse(file);
                    SoundVector sounds;

                    juce::AudioFormatManager manager;
                    manager.registerBasicFormats();

                    for (const auto& zone : parsed.zones) {
                        auto* format = manager.createReaderFor(zone.path);
                        if (!format) continue;

                        std::unique_ptr<Synth::CustomSamplerSound> sound;
                        
                        // Streaming para WAV/AIFF
                        const auto formatName = format->getFormatName();
                        if (formatName == "WAV" || formatName == "AIFF") {
                            if (auto* audioFormat = manager.findFormatForFileExtension(zone.path.getFileExtension())) {
                                if (auto* mappedReader = audioFormat->createMemoryMappedReader(zone.path)) {
                                auto ptr = std::unique_ptr<juce::MemoryMappedAudioFormatReader>(mappedReader);
                                ptr->mapEntireFile();
                                sound = std::make_unique<Synth::CustomSamplerSound>(std::move(ptr));
                                }
                            }
                        }

                        if (!sound) {
                            // RAM fallback for unsupported mapped formats or failed mappings.
                            juce::AudioBuffer<float> buffer(format->numChannels, static_cast<int>(format->lengthInSamples));
                            format->read(&buffer, 0, static_cast<int>(format->lengthInSamples), 0, true, true);
                            sound = std::make_unique<Synth::CustomSamplerSound>(std::move(buffer));
                        }

                        if (sound) {
                            sound->lowNote = zone.loNote;
                            sound->highNote = zone.hiNote;
                            sound->rootNote = zone.rootNote;
                            sounds.push_back(std::move(sound));
                        }
                        delete format;
                    }

                    juce::MessageManager::callAsync([callback, sounds = std::move(sounds)]() {
                        callback(std::move(sounds));
                    });
                });
            }
        };
    }
}
