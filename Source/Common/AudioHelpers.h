#pragma once

#include <cmath>
#include <algorithm>
#include <random>

namespace Floweret
{
    namespace MathConstants
    {
        constexpr float pi = 3.14159265358979323846f;
        constexpr float twoPi = 6.28318530717958647692f;
        constexpr float halfPi = 1.57079632679489661923f;
    }

    //==============================================================================
    // Hermite (cubic) interpolation for smooth continuous wavetable morphing
    inline float hermiteInterpolate (float y0, float y1, float y2, float y3, float frac) noexcept
    {
        const float c0 = y1;
        const float c1 = 0.5f * (y2 - y0);
        const float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        const float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

    // Linear interpolation
    inline float linearInterpolate (float y0, float y1, float frac) noexcept
    {
        return y0 + frac * (y1 - y0);
    }

    // Fast soft saturation curve
    inline float fastTanh (float x) noexcept
    {
        if (x < -3.0f) return -1.0f;
        if (x > 3.0f) return 1.0f;
        return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
    }

    // MIDI Note number to frequency in Hz
    inline float midiToFreq (float midiNote, float a4 = 440.0f) noexcept
    {
        return a4 * std::pow (2.0f, (midiNote - 69.0f) / 12.0f);
    }

    // Frequency in Hz to MIDI note
    inline float freqToMidi (float freq, float a4 = 440.0f) noexcept
    {
        if (freq <= 0.0f) return 0.0f;
        return 69.0f + 12.0f * std::log2 (freq / a4);
    }

    // Simple fast uniform random in [-1, 1]
    class FastRandom
    {
    public:
        FastRandom() : seed (123456789) {}

        inline float nextFloat() noexcept
        {
            seed = (1103515245u * seed + 12345u) & 0x7fffffffu;
            return (static_cast<float> (seed) / 1073741824.0f) - 1.0f;
        }

        inline float nextUnsignedFloat() noexcept
        {
            seed = (1103515245u * seed + 12345u) & 0x7fffffffu;
            return static_cast<float> (seed) / 2147483648.0f;
        }

    private:
        uint32_t seed;
    };
}
