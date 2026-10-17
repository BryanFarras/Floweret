#include "NoiseOscillator.h"

namespace Floweret
{
    NoiseOscillator::NoiseOscillator()
    {
        reset();
    }

    void NoiseOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void NoiseOscillator::reset()
    {
        b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.0f;
        brownState = 0.0f;
        prevWhite = 0.0f;
        blueState = 0.0f;
        greyLp = greyHp = 0.0f;
        velvetCounter = 0;
        velvetTarget = 12;
        velvetCurrentSample = 0.0f;
    }

    float NoiseOscillator::getNextSample (NoiseColor color) noexcept
    {
        const float white = rng.nextFloat();

        switch (color)
        {
            case NoiseColor::White:
                return white * 0.7f;

            case NoiseColor::Pink:
            {
                // Paul Kellet's filter method for Pink Noise (-3dB/oct)
                b0 = 0.99886f * b0 + white * 0.0555179f;
                b1 = 0.99332f * b1 + white * 0.0750759f;
                b2 = 0.96900f * b2 + white * 0.1538520f;
                b3 = 0.86650f * b3 + white * 0.3104856f;
                b4 = 0.55000f * b4 + white * 0.5329522f;
                b5 = -0.7616f * b5 - white * 0.0168980f;
                const float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
                b6 = white * 0.115926f;
                return pink * 0.11f;
            }

            case NoiseColor::Brown:
            {
                // Leaky integration for Brownian Noise (-6dB/oct)
                brownState = 0.985f * brownState + white * 0.08f;
                return brownState * 0.8f;
            }

            case NoiseColor::Blue:
            {
                // Blue noise (+3dB/oct) - gentle differentiation
                const float diff = white - prevWhite;
                blueState = 0.7f * blueState + 0.3f * diff;
                prevWhite = white;
                return blueState * 0.9f;
            }

            case NoiseColor::Violet:
            {
                // Violet noise (+6dB/oct) - full first order difference
                const float violet = (white - prevWhite) * 0.6f;
                prevWhite = white;
                return violet;
            }

            case NoiseColor::Grey:
            {
                // Inverted A-weighting curve filter approximation
                greyLp = 0.92f * greyLp + 0.08f * white;
                greyHp = white - greyLp;
                return (greyHp * 0.7f + greyLp * 0.3f);
            }

            case NoiseColor::Velvet:
            {
                // Velvet noise: Sparse random positive/negative impulses at pseudo-random intervals
                velvetCounter++;
                if (velvetCounter >= velvetTarget)
                {
                    velvetCounter = 0;
                    // randomized interval for next tap
                    velvetTarget = 8 + static_cast<int> (rng.nextUnsignedFloat() * 16.0f);
                    // random positive or negative impulse
                    velvetCurrentSample = (rng.nextFloat() > 0.0f) ? 0.9f : -0.9f;
                }
                else
                {
                    // rapid decay of the impulse
                    velvetCurrentSample *= 0.82f;
                }
                return velvetCurrentSample;
            }

            default:
                return white * 0.7f;
        }
    }
}
