#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class WavetableOscillator
    {
    public:
        static constexpr int tableSize = 2048; // 2048 samples per frame (industry standard)
        static constexpr int numFrames = 64;   // 64 frames per table

        WavetableOscillator();
        ~WavetableOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setFrequency (float freq);

        // Generates sample. Can optionally accept an FM modulator signal
        float getNextSample (float wtPosition, WarpMode warpMode, float warpAmount,
                             bool smoothInterp, float fmInput = 0.0f) noexcept;

        // Provide access to current wavetable frame for UI visualization
        void getWaveformForDisplay (float position, float* destBuffer, int numSamples) const;

    private:
        void generateFactoryWavetables();

        double currentSampleRate = 44100.0;
        float currentFrequency = 220.0f;
        float phase = 0.0f;
        float syncPhase = 0.0f;

        // Multi-frame table: [frame][sample]
        std::vector<std::vector<float>> frames;

        // Warp functions
        float applyWarp (float inPhase, WarpMode mode, float amount, float fmInput) const noexcept;
    };
}
