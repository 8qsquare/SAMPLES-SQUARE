#pragma once
#include <atomic>
#include <juce_core/juce_core.h>

namespace SampleSquare {
    namespace Synth {
        struct ParamsSnapshot {
            float a, d, s, r, g;
        };

        class SharedParams {
        public:
            std::atomic<float> attack  {0.01f};
            std::atomic<float> decay   {0.10f};
            std::atomic<float> sustain {0.80f};
            std::atomic<float> release {0.20f};
            std::atomic<float> gain    {0.70f};

            ParamsSnapshot snapshot() const noexcept {
                return {
                    attack.load(std::memory_order_relaxed),
                    decay.load(std::memory_order_relaxed),
                    sustain.load(std::memory_order_relaxed),
                    release.load(std::memory_order_relaxed),
                    gain.load(std::memory_order_relaxed)
                };
            }
        };
    }
}
