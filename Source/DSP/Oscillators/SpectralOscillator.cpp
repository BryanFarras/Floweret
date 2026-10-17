#include "SpectralOscillator.h"

namespace Floweret
{
    SpectralOscillator::SpectralOscillator()
    {
        partials.resize (maxPartials);
        reset();
    }

    void SpectralOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void SpectralOscillator::reset()
    {
        for (auto& p : partials)
        {
            p.phase = 0.0f;
            p.magnitude = 0.0f;
            p.smoothedMag = 0.0f;
            p.currentFreq = fundamentalFreq;
            p.targetFreq = fundamentalFreq;
        }
    }

    void SpectralOscillator::setFrequency (float baseFreq)
    {
        fundamentalFreq = std::max (10.0f, std::min (baseFreq, 10000.0f));
    }

    float SpectralOscillator::quantizeFrequencyToScale (float freq, MusicalScale currentScale, int rootKey) const
    {
        const float midiVal = freqToMidi (freq);
        const int midiInt = static_cast<int> (std::round (midiVal));
        const int noteInOctave = ((midiInt - rootKey) % 12 + 12) % 12;

        const auto intervals = getScaleIntervals (currentScale);

        // Find closest scale degree
        int bestDiff = 99;
        int bestInterval = 0;
        for (int interval : intervals)
        {
            const int diff = std::abs (noteInOctave - interval);
            if (diff < bestDiff)
            {
                bestDiff = diff;
                bestInterval = interval;
            }
        }

        const int octave = (midiInt - rootKey - noteInOctave);
        const int quantizedMidi = rootKey + octave + bestInterval;
        return midiToFreq (static_cast<float> (quantizedMidi));
    }

    void SpectralOscillator::updatePartials (int numHarmonics, float quantizeAmount, float spectralTilt,
                                            float spectralSmear, MusicalScale currentScale, int rootKey)
    {
        const int activeCount = std::min (numHarmonics, maxPartials);
        const float smearCoeff = 0.95f * spectralSmear;

        for (int i = 0; i < maxPartials; ++i)
        {
            auto& p = partials[i];
            if (i < activeCount)
            {
                const float harmonicNumber = static_cast<float> (i + 1);
                const float rawFreq = fundamentalFreq * harmonicNumber;

                // Color Bass Scale Quantization
                if (quantizeAmount > 0.001f)
                {
                    const float tunedFreq = quantizeFrequencyToScale (rawFreq, currentScale, rootKey);
                    p.targetFreq = linearInterpolate (rawFreq, tunedFreq, quantizeAmount);
                }
                else
                {
                    p.targetFreq = rawFreq;
                }

                // Smooth frequency slew
                p.currentFreq = linearInterpolate (p.currentFreq, p.targetFreq, 0.1f);

                // Magnitude based on natural 1/n decay + spectral tilt
                float baseMag = 1.0f / harmonicNumber;
                // Spectral tilt: positive tilts towards high frequencies, negative tilts towards low
                const float tiltWeight = std::pow (harmonicNumber, spectralTilt * 1.5f);
                float targetMag = baseMag * tiltWeight;

                // Smear smoothing
                p.smoothedMag = smearCoeff * p.smoothedMag + (1.0f - smearCoeff) * targetMag;
                p.magnitude = p.smoothedMag;
            }
            else
            {
                p.magnitude = 0.0f;
            }
        }
    }

    float SpectralOscillator::getNextSample (int numHarmonics, float quantizeAmount, float spectralTilt,
                                            float spectralSmear, MusicalScale currentScale, int rootKey) noexcept
    {
        updatePartials (numHarmonics, quantizeAmount, spectralTilt, spectralSmear, currentScale, rootKey);

        float out = 0.0f;
        const float sampleRateInv = 1.0f / static_cast<float> (currentSampleRate);

        for (auto& p : partials)
        {
            if (p.magnitude > 0.0001f && p.currentFreq < (currentSampleRate * 0.48f))
            {
                out += std::sin (p.phase) * p.magnitude;

                p.phase += p.currentFreq * MathConstants::twoPi * sampleRateInv;
                if (p.phase >= MathConstants::twoPi)
                    p.phase -= MathConstants::twoPi;
            }
        }

        return out * 0.4f;
    }

    void SpectralOscillator::getMagnitudesForDisplay (float* destBuffer, int maxBins) const
    {
        if (destBuffer == nullptr || maxBins <= 0)
            return;

        for (int i = 0; i < maxBins; ++i)
        {
            if (i < static_cast<int> (partials.size()))
                destBuffer[i] = partials[i].magnitude;
            else
                destBuffer[i] = 0.0f;
        }
    }
}
