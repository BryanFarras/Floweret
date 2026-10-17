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
            b.x1L = b.x2L = b.y1L = b.y2L = 0.0f;
            b.x1R = b.x2R = b.y1R = b.y2R = 0.0f;
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
        const float targetQ = std::max (1.0f, resonance);
        if (scale == currentScale && rootKey == currentRootKey && std::abs (targetQ - currentQ) < 0.01f && !bands.empty())
        {
            return;
        }

        currentScale = scale;
        currentRootKey = rootKey;
        currentQ = targetQ;

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
        float l = inputSample;
        float r = inputSample;
        processStereo (l, r, colorAmount, spectralMorph, spectralGate);
        return l;
    }

    void ColorResonator::processStereo (float& inOutL, float& inOutR, float colorAmount, float spectralMorph, float spectralGate) noexcept
    {
        dryEnergy = 0.95f * dryEnergy + 0.05f * (std::abs (inOutL) + std::abs (inOutR)) * 0.5f;

        if (colorAmount < 0.01f || bands.empty())
            return;

        float sumL = 0.0f;
        float sumR = 0.0f;
        const float boostFactor = (colorAmount > 100.0f) ? (1.0f + (colorAmount - 100.0f) * 0.03f) : 1.0f;

        for (auto& b : bands)
        {
            // Left channel
            const float ynL = b.b0 * inOutL + b.b1 * b.x1L + b.b2 * b.x2L - b.a1 * b.y1L - b.a2 * b.y2L;
            b.x2L = b.x1L; b.x1L = inOutL;
            b.y2L = b.y1L; b.y1L = ynL;
            sumL += ynL;

            // Right channel
            const float ynR = b.b0 * inOutR + b.b1 * b.x1R + b.b2 * b.x2R - b.a1 * b.y1R - b.a2 * b.y2R;
            b.x2R = b.x1R; b.x1R = inOutR;
            b.y2R = b.y1R; b.y1R = ynR;
            sumR += ynR;
        }

        const float norm = 1.8f * boostFactor / std::sqrt (static_cast<float> (bands.size()));
        sumL *= norm;
        sumR *= norm;

        // Apply spectral gate if energy is too low
        const float gateThreshold = std::pow (10.0f, spectralGate / 20.0f);
        if (std::abs (sumL) < gateThreshold) sumL *= 0.1f;
        if (std::abs (sumR) < gateThreshold) sumR *= 0.1f;

        // Color Bass hyper-resonance saturation for values 100-200%
        if (colorAmount > 100.0f)
        {
            sumL = fastTanh (sumL * 1.4f);
            sumR = fastTanh (sumR * 1.4f);
        }

        tunedEnergy = 0.95f * tunedEnergy + 0.05f * (std::abs (sumL) + std::abs (sumR)) * 0.5f;

        // Wet / Dry blend
        const float dryWetMix = std::min (colorAmount, 100.0f) * 0.01f;
        inOutL = linearInterpolate (inOutL, sumL, dryWetMix);
        inOutR = linearInterpolate (inOutR, sumR, dryWetMix);

        // Spectral Morph blending
        if (spectralMorph > 0.01f)
        {
            inOutL = linearInterpolate (inOutL, sumL, spectralMorph * 0.5f);
            inOutR = linearInterpolate (inOutR, sumR, spectralMorph * 0.5f);
        }
    }
}
