#pragma once

#include <JuceHeader.h>
#include "../../Common/AudioHelpers.h"

namespace Floweret
{
    class SubOscillator
    {
    public:
        SubOscillator();
        ~SubOscillator() = default;

        void prepare (double sampleRate);
        void reset();
        void setFrequency (float freq);

        float getNextSample (int octaveOffset, float drive, int waveform = 0) noexcept;

    private:
        double currentSampleRate = 44100.0;
        float currentFrequency = 110.0f;
        float phase = 0.0f;
    };
}
