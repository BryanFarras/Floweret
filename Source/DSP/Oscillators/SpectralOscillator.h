#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Common/Parameters.h"
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    struct SpectralPartial
    {
        float targetFreq = 440.0f;
        float currentFreq = 440.0f;
        float magnitude = 0.0f;
        float smoothedMag = 0.0f;
        float phase = 0.0f;
    };

    class SpectralOscillator
    {
    public:
        static constexpr int maxPartials = 64;

        SpectralOscillator();
        ~SpectralOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setFrequency (float baseFreq);

        // Process audio sample
        float getNextSample (int numHarmonics, float quantizeAmount, float spectralTilt,
                             float spectralSmear, MusicalScale currentScale, int rootKey) noexcept;

        // Provide partial magnitudes for spectral UI visualizer
        void getMagnitudesForDisplay (float* destBuffer, int maxBins) const;

    private:
        void updatePartials (int numHarmonics, float quantizeAmount, float spectralTilt,
                             float spectralSmear, MusicalScale currentScale, int rootKey);

        float quantizeFrequencyToScale (float freq, MusicalScale currentScale, int rootKey) const;

        double currentSampleRate = 44100.0;
        float fundamentalFreq = 220.0f;
        std::vector<SpectralPartial> partials;
    };
}
