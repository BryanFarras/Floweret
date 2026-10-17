#include "MultiModeFilter.h"

namespace Floweret
{
    MultiModeFilter::MultiModeFilter()
    {
        combBuffer.resize (2048, 0.0f);
        reset();
    }

    void MultiModeFilter::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        combBuffer.resize (static_cast<size_t> (sampleRate * 0.05) + 16, 0.0f);
        reset();
    }

    void MultiModeFilter::reset()
    {
        s1 = s2 = s3 = s4 = 0.0f;
        std::fill (combBuffer.begin(), combBuffer.end(), 0.0f);
        combWriteIndex = 0;
        f1_s1 = f1_s2 = f2_s1 = f2_s2 = 0.0f;
    }

    float MultiModeFilter::processSample (float input, FilterType type, float cutoffHz, float resonance, float drive) noexcept
    {
        // Apply drive
        float drivenInput = input;
        if (drive > 0.01f)
            drivenInput = fastTanh (input * (1.0f + drive * 3.0f));

        const float safeCutoff = std::max (20.0f, std::min (cutoffHz, static_cast<float> (currentSampleRate * 0.45f)));

        if (type == FilterType::Comb)
        {
            // Tuned Comb Filter
            const float delayTimeSec = 1.0f / safeCutoff;
            const int delaySamples = std::max (1, std::min (static_cast<int> (delayTimeSec * currentSampleRate),
                                                            static_cast<int> (combBuffer.size() - 2)));

            int readIdx = combWriteIndex - delaySamples;
            if (readIdx < 0) readIdx += static_cast<int> (combBuffer.size());

            const float delayedSample = combBuffer[static_cast<size_t> (readIdx)];
            const float feedback = std::min (0.95f, 0.2f + resonance * 0.15f);
            const float newCombSample = drivenInput + delayedSample * feedback;

            combBuffer[static_cast<size_t> (combWriteIndex)] = newCombSample;
            combWriteIndex = (combWriteIndex + 1) % static_cast<int> (combBuffer.size());

            return (drivenInput + delayedSample) * 0.5f;
        }

        if (type == FilterType::FormantVowel)
        {
            // Dual formant peaks (simulating vowel /a/ -> /o/ timbre)
            const float f1 = std::min (safeCutoff * 0.8f, 1200.0f);
            const float f2 = std::min (safeCutoff * 2.2f, 3200.0f);

            // BPF 1
            const float g1 = std::tan (MathConstants::pi * f1 / static_cast<float> (currentSampleRate));
            const float k1 = 1.0f / (resonance * 2.5f);
            const float hp1 = (drivenInput - (2.0f * k1 + g1) * f1_s1 - f1_s2) / (1.0f + 2.0f * k1 * g1 + g1 * g1);
            const float bp1 = hp1 * g1 + f1_s1;
            f1_s1 = bp1 * g1 + bp1;
            f1_s2 = bp1 * g1 + f1_s2;

            // BPF 2
            const float g2 = std::tan (MathConstants::pi * f2 / static_cast<float> (currentSampleRate));
            const float k2 = 1.0f / (resonance * 2.5f);
            const float hp2 = (drivenInput - (2.0f * k2 + g2) * f2_s1 - f2_s2) / (1.0f + 2.0f * k2 * g2 + g2 * g2);
            const float bp2 = hp2 * g2 + f2_s1;
            f2_s1 = bp2 * g2 + bp2;
            f2_s2 = bp2 * g2 + f2_s2;

            return (bp1 * 0.6f + bp2 * 0.4f) * 1.5f;
        }

        // Standard State Variable Filter (SVF) with trapezoidal integration (Chamberlin / Zavalishin topology)
        const float g = std::tan (MathConstants::pi * safeCutoff / static_cast<float> (currentSampleRate));
        const float k = 1.0f / std::max (0.1f, resonance);

        // Stage 1
        const float hp = (drivenInput - (2.0f * k + g) * s1 - s2) / (1.0f + 2.0f * k * g + g * g);
        const float bp = hp * g + s1;
        s1 = bp * g + bp;
        const float lp = bp * g + s2;
        s2 = lp * g + lp;
        const float notch = hp + lp;

        // If 12dB mode, return immediately
        if (type == FilterType::LowPass12)  return lp;
        if (type == FilterType::HighPass12) return hp;
        if (type == FilterType::BandPass)   return bp;
        if (type == FilterType::Notch)      return notch;

        // Stage 2 for 24dB slopes
        float inputStage2 = (type == FilterType::LowPass24) ? lp : hp;
        const float hp2 = (inputStage2 - (2.0f * k + g) * s3 - s4) / (1.0f + 2.0f * k * g + g * g);
        const float bp2 = hp2 * g + s3;
        s3 = bp2 * g + bp2;
        const float lp2 = bp2 * g + s4;
        s4 = lp2 * g + lp2;

        if (type == FilterType::LowPass24)  return lp2;
        if (type == FilterType::HighPass24) return hp2;

        return lp;
    }
}
