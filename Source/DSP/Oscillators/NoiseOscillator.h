#pragma once

#include <JuceHeader.h>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class NoiseOscillator
    {
    public:
        NoiseOscillator();
        ~NoiseOscillator() = default;

        void prepare (double sampleRate);
        void reset();

        float getNextSample (NoiseColor color) noexcept;

    private:
        double currentSampleRate = 44100.0;
        FastRandom rng;

        // Pink noise filter states (Kellet algorithm)
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;

        // Brown noise state
        float brownState = 0.0f;

        // Violet noise state
        float prevWhite = 0.0f;

        // Blue noise state
        float blueState = 0.0f;

        // Grey noise filter states (psychoacoustic inverted equal-loudness)
        float greyLp = 0.0f;
        float greyHp = 0.0f;

        // Velvet noise state (sparse random positive/negative impulses)
        int velvetCounter = 0;
        int velvetTarget = 10;
        float velvetCurrentSample = 0.0f;
    };
}
