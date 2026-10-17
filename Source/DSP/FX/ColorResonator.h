#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct ResonatorBand
    {
        float frequency = 440.0f;
        // Biquad bandpass filter states
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };

    class ColorResonator
    {
    public:
        ColorResonator();
        ~ColorResonator() = default;

        void prepare (double sampleRate);
        void reset();

        void updateScale (MusicalScale scale, int rootKey, float resonance);

        // Process audio sample
        float processSample (float inputSample, float colorAmount, float spectralMorph, float spectralGate) noexcept;

        // Provide real-time data for the Chroma-style visualizer
        float getDryMagnitude() const { return dryEnergy; }
        float getTunedMagnitude() const { return tunedEnergy; }
        const std::vector<ResonatorBand>& getBands() const { return bands; }

    private:
        void recalculateCoefficients (ResonatorBand& band, float q);

        double currentSampleRate = 44100.0;
        MusicalScale currentScale = MusicalScale::Major;
        int currentRootKey = 3; // Eb
        float currentQ = 8.0f;

        std::vector<ResonatorBand> bands;

        float dryEnergy = 0.0f;
        float tunedEnergy = 0.0f;
    };
}
