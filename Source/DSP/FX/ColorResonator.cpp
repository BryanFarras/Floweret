#include "ColorResonator.h"

namespace Floweret
{
    ColorResonator::ColorResonator()
    {
        updateScale (MusicalScale::Major, 3, 8.0f);
        reset();
    }

    void ColorResonator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        updateScale (currentScale, currentRootKey, currentQ);
        reset();
    }

    void ColorResonator::reset()
    {
        for (auto& b : bands)
        {
            b.x1 = b.x2 = b.y1 = b.y2 = 0.0f;
        }
        dryEnergy = 0.0f;
        tunedEnergy = 0.0f;
    }

    void ColorResonator::recalculateCoefficients (ResonatorBand& band, float q)
    {
        const float w0 = MathConstants::twoPi * band.frequency / static_cast<float> (currentSampleRate);
        const float cosw0 = std::cos (w0);
        const float sinw0 = std::sin (w0);
        const float alpha = sinw0 / (2.0f * q);

        const float a0 = 1.0f + alpha;
        band.b0 = alpha / a0;
        band.b1 = 0.0f;
        band.b2 = -alpha / a0;
        band.a1 = (-2.0f * cosw0) / a0;
        band.a2 = (1.0f - alpha) / a0;
    }

    void ColorResonator::updateScale (MusicalScale scale, int rootKey, float resonance)
    {
        currentScale = scale;
        currentRootKey = rootKey;
        currentQ = std::max (1.0f, resonance);

        bands.clear();
        const auto intervals = getScaleIntervals (scale);

        // Populate resonant bands for all scale degrees across 6 octaves (MIDI 36 to 108: ~65 Hz to 4200 Hz)
        for (int oct = 2; oct <= 8; ++oct)
        {
            for (int interval : intervals)
            {
                const int midiNote = rootKey + (oct * 12) + interval;
                if (midiNote >= 36 && midiNote <= 108)
                {
                    const float freq = midiToFreq (static_cast<float> (midiNote));
                    if (freq > 80.0f && freq < (currentSampleRate * 0.45f))
                    {
                        ResonatorBand band;
                        band.frequency = freq;
                        recalculateCoefficients (band, currentQ);
                        bands.push_back (band);
                    }
                }
            }
        }
    }

    float ColorResonator::processSample (float inputSample, float colorAmount, float spectralMorph, float spectralGate) noexcept
    {
        dryEnergy = 0.95f * dryEnergy + 0.05f * std::abs (inputSample);

        if (colorAmount < 0.01f || bands.empty())
            return inputSample;

        // Effective Q and drive multiplier based on Color parameter (0 to 200%)
        // Values > 100% boost resonance and add harmonic saturation
        float resonantSum = 0.0f;
        const float boostFactor = (colorAmount > 100.0f) ? (1.0f + (colorAmount - 100.0f) * 0.03f) : 1.0f;

        for (auto& b : bands)
        {
            // Biquad bandpass filter equation
            const float yn = b.b0 * inputSample + b.b1 * b.x1 + b.b2 * b.x2
                             - b.a1 * b.y1 - b.a2 * b.y2;

            b.x2 = b.x1;
            b.x1 = inputSample;
            b.y2 = b.y1;
            b.y1 = yn;

            resonantSum += yn;
        }

        // Normalize sum by number of active resonant bands
        resonantSum = (resonantSum / std::sqrt (static_cast<float> (bands.size()))) * 1.8f * boostFactor;

        // Apply spectral gate if energy is too low
        const float gateThreshold = std::pow (10.0f, spectralGate / 20.0f);
        if (std::abs (resonantSum) < gateThreshold)
        {
            resonantSum *= 0.1f;
        }

        // Color Bass hyper-resonance saturation for values 100-200%
        if (colorAmount > 100.0f)
        {
            resonantSum = fastTanh (resonantSum * 1.4f);
        }

        tunedEnergy = 0.95f * tunedEnergy + 0.05f * std::abs (resonantSum);

        // Wet / Dry blend
        const float dryWetMix = std::min (colorAmount, 100.0f) * 0.01f;
        float output = linearInterpolate (inputSample, resonantSum, dryWetMix);

        // Spectral Morph blending
        if (spectralMorph > 0.01f)
        {
            // Morph preserves transients from dry while carrying resonant body
            output = linearInterpolate (output, resonantSum, spectralMorph * 0.5f);
        }

        return output;
    }
}
