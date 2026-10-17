#include "GranularOscillator.h"

namespace Floweret
{
    GranularOscillator::GranularOscillator()
    {
        grains.resize (maxGrains);
        generateBotanicaSample();
        reset();
    }

    void GranularOscillator::prepare (double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void GranularOscillator::reset()
    {
        for (auto& g : grains)
            g.active = false;

        grainSpawnAccumulator = 0.0f;
    }

    void GranularOscillator::setFrequency (float freq)
    {
        currentFrequency = std::max (10.0f, std::min (freq, 20000.0f));
    }

    void GranularOscillator::generateBotanicaSample()
    {
        // Generate a 4-second organic botanical soundscape buffer:
        // Harmonic bells, warm resonant blooms, and gentle acoustic texture
        const int numSamples = static_cast<int> (currentSampleRate * 4.0);
        sampleBuffer.setSize (1, numSamples);
        auto* data = sampleBuffer.getWritePointer (0);

        for (int i = 0; i < numSamples; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (currentSampleRate);

            // Layer 1: Ethereal bell tones (partials at 1x, 2.76x, 5.4x, 8.9x)
            const float bell1 = std::sin (t * MathConstants::twoPi * 261.63f) * std::exp (-t * 0.8f);
            const float bell2 = std::sin (t * MathConstants::twoPi * 261.63f * 2.76f) * 0.4f * std::exp (-t * 1.5f);
            const float bell3 = std::sin (t * MathConstants::twoPi * 261.63f * 5.40f) * 0.2f * std::exp (-t * 2.8f);

            // Layer 2: Warm organic swelling chord (C - Eb - G - Bb)
            const float envSwell = 0.5f - 0.5f * std::cos (t * MathConstants::twoPi * 0.25f);
            const float chord = (std::sin (t * MathConstants::twoPi * 130.81f)
                               + std::sin (t * MathConstants::twoPi * 155.56f)
                               + std::sin (t * MathConstants::twoPi * 196.00f)
                               + std::sin (t * MathConstants::twoPi * 233.08f)) * 0.15f * envSwell;

            // Layer 3: Velvet organic rustle / water droplets
            const float rustle = (std::sin (t * 880.0f * MathConstants::twoPi) * std::sin (t * 14.0f)) * 0.05f;

            data[i] = fastTanh (bell1 + bell2 + bell3 + chord + rustle);
        }
    }

    void GranularOscillator::loadSample (const juce::AudioBuffer<float>& newBuffer)
    {
        if (newBuffer.getNumSamples() > 0)
        {
            sampleBuffer.makeCopyOf (newBuffer);
        }
    }

    float GranularOscillator::getBufferLengthSeconds() const
    {
        if (sampleBuffer.getNumSamples() <= 0 || currentSampleRate <= 0)
            return 0.0f;

        return static_cast<float> (sampleBuffer.getNumSamples()) / static_cast<float> (currentSampleRate);
    }

    void GranularOscillator::spawnGrain (float positionNorm, float grainSizeMs, float spray, float pitchJitterSemitones)
    {
        // Find an inactive grain slot
        for (auto& g : grains)
        {
            if (!g.active)
            {
                g.active = true;
                g.currentSample = 0;

                // Duration in samples
                const float durSec = (grainSizeMs * 0.001f);
                g.lengthSamples = std::max (64, static_cast<int> (durSec * currentSampleRate));

                // Position calculation with spray jitter
                const int bufLen = sampleBuffer.getNumSamples();
                float centerPos = positionNorm * static_cast<float> (bufLen);
                float offset = spray * rng.nextFloat() * (currentSampleRate * 0.5f);
                float startSample = centerPos + offset;

                // Wrap or clamp within buffer
                if (startSample < 0.0f) startSample = 0.0f;
                if (startSample >= static_cast<float> (bufLen - g.lengthSamples - 1))
                    startSample = static_cast<float> (std::max (0, bufLen - g.lengthSamples - 2));

                g.bufferPosition = startSample;

                // Base speed from note frequency relative to Middle C (261.63Hz)
                float baseSpeed = currentFrequency / 261.63f;

                // Pitch jitter
                if (pitchJitterSemitones > 0.01f)
                {
                    const float jitter = rng.nextFloat() * pitchJitterSemitones;
                    baseSpeed *= std::pow (2.0f, jitter / 12.0f);
                }

                g.speed = baseSpeed;

                // Random stereo panning
                const float pan = rng.nextFloat() * 0.8f; // -0.8 to +0.8
                const float panAngle = (pan + 1.0f) * 0.25f * MathConstants::pi; // 0 to pi/2
                g.panLeft = std::cos (panAngle);
                g.panRight = std::sin (panAngle);

                break;
            }
        }
    }

    std::pair<float, float> GranularOscillator::getNextSample (float positionNorm, float grainSizeMs,
                                                              float density, float spray,
                                                              float pitchJitterSemitones, bool freeze) noexcept
    {
        if (sampleBuffer.getNumSamples() <= 0)
            return { 0.0f, 0.0f };

        // Spawn accumulator based on density (grains per second)
        const float spawnRate = density / static_cast<float> (currentSampleRate);
        grainSpawnAccumulator += spawnRate;

        while (grainSpawnAccumulator >= 1.0f)
        {
            grainSpawnAccumulator -= 1.0f;
            spawnGrain (positionNorm, grainSizeMs, spray, pitchJitterSemitones);
        }

        float outLeft = 0.0f;
        float outRight = 0.0f;
        const auto* buf = sampleBuffer.getReadPointer (0);
        const int bufLen = sampleBuffer.getNumSamples();

        for (auto& g : grains)
        {
            if (g.active)
            {
                // Hann window calculation
                const float windowPhase = static_cast<float> (g.currentSample) / static_cast<float> (g.lengthSamples);
                const float window = 0.5f - 0.5f * std::cos (windowPhase * MathConstants::twoPi);

                // Sample reading with linear interpolation
                const int idx0 = static_cast<int> (g.bufferPosition);
                const int idx1 = std::min (idx0 + 1, bufLen - 1);
                const float frac = g.bufferPosition - static_cast<float> (idx0);

                const float rawSample = linearInterpolate (buf[idx0], buf[idx1], frac);
                const float grainSample = rawSample * window;

                outLeft += grainSample * g.panLeft;
                outRight += grainSample * g.panRight;

                // Advance grain
                if (!freeze)
                {
                    g.bufferPosition += g.speed;
                    if (g.bufferPosition >= static_cast<float> (bufLen - 1))
                        g.bufferPosition = 0.0f;
                }
                else
                {
                    // In freeze mode, playback oscillates or stays tightly clamped
                    g.bufferPosition += g.speed * 0.05f;
                }

                g.currentSample++;
                if (g.currentSample >= g.lengthSamples)
                {
                    g.active = false;
                }
            }
        }

        // Scale by square root of active grains for uniform loudness
        return { outLeft * 0.35f, outRight * 0.35f };
    }
}
