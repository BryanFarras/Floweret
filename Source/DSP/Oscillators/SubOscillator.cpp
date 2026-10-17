#include "SubOscillator.h"

namespace Floweret
{
    SubOscillator::SubOscillator()
    {
        reset();
    }

    void SubOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void SubOscillator::reset()
    {
        phase = 0.0f;
    }

    void SubOscillator::setFrequency (float freq)
    {
        currentFrequency = freq;
    }

    float SubOscillator::getNextSample (int octaveOffset, float drive, int waveform) noexcept
    {
        // Octave divide factor: 0 -> -1 Oct (div 2), 1 -> -2 Oct (div 4), 2 -> -3 Oct (div 8)
        const float div = std::pow (2.0f, static_cast<float> (octaveOffset + 1));
        const float subFreq = currentFrequency / div;

        const float phaseInc = subFreq / static_cast<float> (currentSampleRate);
        phase += phaseInc;
        if (phase >= 1.0f)
            phase -= 1.0f;

        float out = 0.0f;
        if (waveform == 0) // Sine
        {
            out = std::sin (phase * MathConstants::twoPi);
        }
        else if (waveform == 1) // Triangle
        {
            out = 4.0f * std::abs (phase - 0.5f) - 1.0f;
        }
        else // Square
        {
            out = (phase < 0.5f) ? 1.0f : -1.0f;
        }

        // Apply drive/saturation
        if (drive > 0.01f)
        {
            out = fastTanh (out * (1.0f + drive * 3.5f));
        }

        return out;
    }
}
