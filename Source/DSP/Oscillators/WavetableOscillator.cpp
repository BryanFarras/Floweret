#include "WavetableOscillator.h"

namespace Floweret
{
    WavetableOscillator::WavetableOscillator()
    {
        generateFactoryWavetables();
        reset();
    }

    void WavetableOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void WavetableOscillator::reset()
    {
        phase = 0.0f;
        syncPhase = 0.0f;
    }

    void WavetableOscillator::setFrequency (float freq)
    {
        currentFrequency = std::max (10.0f, std::min (freq, 20000.0f));
    }

    void WavetableOscillator::generateFactoryWavetables()
    {
        frames.resize (numFrames);
        for (int f = 0; f < numFrames; ++f)
        {
            frames[f].resize (tableSize);
            const float morphProgress = static_cast<float> (f) / static_cast<float> (numFrames - 1);

            for (int i = 0; i < tableSize; ++i)
            {
                const float t = static_cast<float> (i) / static_cast<float> (tableSize);
                const float angle = t * MathConstants::twoPi;

                // Wave 1: Pure Sine
                const float sineVal = std::sin (angle);

                // Wave 2: Triangle (sum of odd harmonics)
                float triVal = 0.0f;
                for (int h = 1; h <= 15; h += 2)
                {
                    const float sign = ((h % 4) == 1) ? 1.0f : -1.0f;
                    triVal += sign * (std::sin (angle * static_cast<float> (h)) / (h * h));
                }
                triVal *= 1.25f;

                // Wave 3: Bandlimited Saw (sum of harmonics up to Nyquist limit)
                float sawVal = 0.0f;
                for (int h = 1; h <= 24; ++h)
                {
                    sawVal += std::sin (angle * static_cast<float> (h)) / static_cast<float> (h);
                }
                sawVal *= 0.6f;

                // Wave 4: Modern Color Bass Formant Wave (multi-harmonic peak with vowel timbre)
                float formantVal = 0.0f;
                const float centerHarmonic = 4.0f + morphProgress * 12.0f;
                for (int h = 1; h <= 32; ++h)
                {
                    const float dist = std::abs (static_cast<float> (h) - centerHarmonic);
                    const float weight = std::exp (-0.5f * (dist * dist) / 4.0f);
                    formantVal += weight * std::sin (angle * static_cast<float> (h));
                }
                formantVal *= 0.45f;

                // Morph smoothly across the 64 frames
                float sampleVal = 0.0f;
                if (morphProgress < 0.33f)
                {
                    const float p = morphProgress / 0.33f;
                    sampleVal = linearInterpolate (sineVal, triVal, p);
                }
                else if (morphProgress < 0.66f)
                {
                    const float p = (morphProgress - 0.33f) / 0.33f;
                    sampleVal = linearInterpolate (triVal, sawVal, p);
                }
                else
                {
                    const float p = (morphProgress - 0.66f) / 0.34f;
                    sampleVal = linearInterpolate (sawVal, formantVal, p);
                }

                frames[f][i] = std::max (-1.0f, std::min (sampleVal, 1.0f));
            }
        }
    }

    float WavetableOscillator::applyWarp (float inPhase, WarpMode mode, float amount, float fmInput) const noexcept
    {
        float warped = inPhase;

        switch (mode)
        {
            case WarpMode::PhaseDistortion:
            {
                // Casio CZ-style dual-slope phase distortion
                if (amount > 0.001f)
                {
                    const float inflection = 0.5f - 0.45f * amount;
                    if (warped < inflection)
                        warped = 0.5f * (warped / inflection);
                    else
                        warped = 0.5f + 0.5f * ((warped - inflection) / (1.0f - inflection));
                }
                break;
            }

            case WarpMode::FrequencyModulation:
            {
                // Linear phase modulation
                warped += amount * fmInput * 0.5f;
                break;
            }

            case WarpMode::RingModulation:
            {
                // Handled in sample multiplication, leave phase as is
                break;
            }

            case WarpMode::HardSync:
            {
                // Virtual oscillator hard sync
                const float syncMult = 1.0f + amount * 7.0f;
                warped = std::fmod (warped * syncMult, 1.0f);
                break;
            }

            case WarpMode::BendPlus:
            {
                // Exponential phase bend forward
                if (amount > 0.001f)
                {
                    const float exponent = 1.0f + amount * 4.0f;
                    warped = std::pow (warped, exponent);
                }
                break;
            }

            case WarpMode::BendMinus:
            {
                // Exponential phase bend backwards
                if (amount > 0.001f)
                {
                    const float exponent = 1.0f / (1.0f + amount * 4.0f);
                    warped = std::pow (warped, exponent);
                }
                break;
            }

            case WarpMode::DualWarp:
            {
                // Combine Phase Distortion and Bend
                const float inflection = 0.5f - 0.35f * amount;
                if (warped < inflection)
                    warped = 0.5f * std::pow (warped / inflection, 1.0f + amount * 2.0f);
                else
                    warped = 0.5f + 0.5f * ((warped - inflection) / (1.0f - inflection));
                break;
            }

            default:
                break;
        }

        // Wrap to [0, 1)
        warped = std::fmod (warped, 1.0f);
        if (warped < 0.0f)
            warped += 1.0f;

        return warped;
    }

    float WavetableOscillator::getNextSample (float wtPosition, WarpMode warpMode, float warpAmount,
                                             bool smoothInterp, float fmInput) noexcept
    {
        const float phaseInc = currentFrequency / static_cast<float> (currentSampleRate);
        phase += phaseInc;
        if (phase >= 1.0f)
            phase -= 1.0f;

        const float warpedPhase = applyWarp (phase, warpMode, warpAmount, fmInput);

        // Frame index calculation
        const float clampedPos = std::max (0.0f, std::min (wtPosition, 1.0f));
        const float frameFloat = clampedPos * static_cast<float> (numFrames - 1);
        const int frame0 = static_cast<int> (frameFloat);
        const int frame1 = std::min (frame0 + 1, numFrames - 1);
        const float frameFrac = frameFloat - static_cast<float> (frame0);

        // Sample index within table
        const float sampleFloat = warpedPhase * static_cast<float> (tableSize);
        const int idx1 = static_cast<int> (sampleFloat);
        const int idx0 = (idx1 - 1 + tableSize) % tableSize;
        const int idx2 = (idx1 + 1) % tableSize;
        const int idx3 = (idx1 + 2) % tableSize;
        const float sampleFrac = sampleFloat - static_cast<float> (idx1);

        float sFrame0 = 0.0f;
        float sFrame1 = 0.0f;

        if (smoothInterp)
        {
            // Cubic Hermite interpolation inside frame 0
            sFrame0 = hermiteInterpolate (frames[frame0][idx0], frames[frame0][idx1],
                                          frames[frame0][idx2], frames[frame0][idx3], sampleFrac);

            // Cubic Hermite interpolation inside frame 1
            sFrame1 = hermiteInterpolate (frames[frame1][idx0], frames[frame1][idx1],
                                          frames[frame1][idx2], frames[frame1][idx3], sampleFrac);
        }
        else
        {
            // Linear interpolation
            sFrame0 = linearInterpolate (frames[frame0][idx1], frames[frame0][idx2], sampleFrac);
            sFrame1 = linearInterpolate (frames[frame1][idx1], frames[frame1][idx2], sampleFrac);
        }

        // Interpolate across frames for continuous position
        float finalSample = linearInterpolate (sFrame0, sFrame1, frameFrac);

        if (warpMode == WarpMode::RingModulation && warpAmount > 0.001f)
        {
            const float rmCarrier = std::sin (phase * MathConstants::twoPi * 2.0f);
            finalSample = linearInterpolate (finalSample, finalSample * rmCarrier, warpAmount);
        }

        return finalSample;
    }

    void WavetableOscillator::getWaveformForDisplay (float position, float* destBuffer, int numSamples) const
    {
        if (destBuffer == nullptr || numSamples <= 0 || frames.empty())
            return;

        const float clampedPos = std::max (0.0f, std::min (position, 1.0f));
        const float frameFloat = clampedPos * static_cast<float> (numFrames - 1);
        const int frame0 = static_cast<int> (frameFloat);
        const int frame1 = std::min (frame0 + 1, numFrames - 1);
        const float frameFrac = frameFloat - static_cast<float> (frame0);

        for (int i = 0; i < numSamples; ++i)
        {
            const float phaseRatio = static_cast<float> (i) / static_cast<float> (numSamples);
            const float sampleFloat = phaseRatio * static_cast<float> (tableSize);
            const int idx = static_cast<int> (sampleFloat) % tableSize;
            const int nextIdx = (idx + 1) % tableSize;
            const float frac = sampleFloat - static_cast<float> (idx);

            const float v0 = linearInterpolate (frames[frame0][idx], frames[frame0][nextIdx], frac);
            const float v1 = linearInterpolate (frames[frame1][idx], frames[frame1][nextIdx], frac);
            destBuffer[i] = linearInterpolate (v0, v1, frameFrac);
        }
    }
}
